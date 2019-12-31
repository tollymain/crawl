/**
 * @file
 * @brief Self-enchantment spells.
**/

#include "AppHdr.h"

#include "spl-selfench.h"

#include <cmath>

#include "act-iter.h"
#include "areas.h"
#include "art-enum.h"
#include "butcher.h" // butcher_corpse
#include "coordit.h" // radius_iterator
#include "god-conduct.h"
#include "god-passive.h"
#include "hints.h"
#include "items.h" // stack_iterator
#include "libutil.h"
#include "macro.h"
#include "message.h"
#include "monster.h"
#include "mutation.h"
#include "output.h"
#include "player.h"
#include "religion.h"
#include "showsymb.h"
#include "spl-summoning.h"
#include "spl-transloc.h"
#include "spl-util.h"
#include "spl-wpnench.h"
#include "terrain.h"
#include "transform.h"
#include "tilepick.h"
#include "view.h"
#include "viewchar.h"

int allowed_deaths_door_hp()
{
    int hp = calc_spell_power(SPELL_DEATHS_DOOR, true) / 10;

    return max(hp, 1);
}

spret_type cast_deaths_door(int pow, bool fail)
{
    fail_check();
    mpr("You stand defiantly in death's doorway!");
    mprf(MSGCH_SOUND, "You seem to hear sand running through an hourglass...");

    set_hp(allowed_deaths_door_hp());
    deflate_hp(you.hp_max, false);

    you.set_duration(DUR_DEATHS_DOOR, 10 + random2avg(13, 3)
                                       + (random2(pow) / 10));

    if (you.duration[DUR_DEATHS_DOOR] > 25 * BASELINE_DELAY)
        you.duration[DUR_DEATHS_DOOR] = (23 + random2(5)) * BASELINE_DELAY;
    return SPRET_SUCCESS;
}

void remove_ice_armour()
{
    // mprf(MSGCH_DURATION, "Your icy armour melts away.");
    you.redraw_armour_class = true;
    spell_remove_permabuff(SPELL_OZOCUBUS_ARMOUR, 3);
}

spret_type ice_armour(int pow, bool fail)
{
    fail_check();

    if(!you.permabuffs[MUT_OZOCUBUS_ARMOUR])
    {
        if(spell_add_permabuff(SPELL_OZOCUBUS_ARMOUR, 3))
        {
            if (you.form == transformation::ice_beast)
                mpr("Your icy body feels more resilient.");

            if (you.attribute[ATTR_BONE_ARMOUR] > 0)
            {
                you.attribute[ATTR_BONE_ARMOUR] = 0;
                mpr("Your corpse armour falls away.");
            }

            you.props[ICY_ARMOUR_KEY] = pow;
            you.redraw_armour_class = true;

            return SPRET_SUCCESS;
        }
        else
        {
            return SPRET_ABORT;
        }
    }       
    else
    {
        spell_remove_permabuff(SPELL_OZOCUBUS_ARMOUR, 3);
        return SPRET_ABORT;
    }
}

/**
 * Iterate over all corpses in LOS and harvest them (unless it's just a test
 * run)
 *
 * @param harvester   The entity planning to do the harvesting.
 * @param dry_run     Whether this is a test run & no corpses should be
 *                    actually destroyed.
 * @param defy_god    Whether to ignore religious restrictions on defiling
 *                    corpses.
 * @return            The total number of corpses (available to be) destroyed.
 */
int harvest_corpses(const actor &harvester, bool dry_run, bool defy_god)
{
    int harvested = 0;

    for (radius_iterator ri(harvester.pos(), LOS_NO_TRANS); ri; ++ri)
    {
        for (stack_iterator si(*ri, true); si; ++si)
        {
            item_def &item = *si;
            if (item.base_type != OBJ_CORPSES)
                continue;

            // forbid harvesting orcs under Beogh
            if (you.religion == GOD_BEOGH && !defy_god)
            {
                const monster_type monnum
                    = static_cast<monster_type>(item.orig_monnum);
                if (mons_genus(monnum) == MONS_ORC)
                    continue;
            }

            ++harvested;

            if (dry_run)
                continue;

            // don't spam animations
            if (harvested <= 5)
            {
                bolt beam;
                beam.source = *ri;
                beam.target = harvester.pos();
                beam.glyph = get_item_glyph(item).ch;
                beam.colour = item.get_colour();
                beam.range = LOS_RADIUS;
                beam.aimed_at_spot = true;
                beam.item = &item;
                beam.flavour = BEAM_VISUAL;
                beam.draw_delay = 3;
                beam.fire();
                viewwindow();
            }

            destroy_item(item.index());
        }
    }

    return harvested;
}

/**
 * Casts the player spell "Cigotuvi's Embrace", pulling all corpses into LOS
 * around the caster to serve as armour.
 *
 * @param pow   The spellpower at which the spell is being cast.
 * @param fail  Whether the casting failed.
 * @return      SPRET_ABORT if you already have an incompatible buff running,
 *              SPRET_FAIL if fail is true, and SPRET_SUCCESS otherwise.
 */
spret_type corpse_armour(int pow, bool fail)
{
    // Could check carefully to see if it's even possible that there are any
    // valid corpses/skeletons in LOS (any piles with stuff under them, etc)
    // before failing, but it's better to be simple + predictable from the
    // player's perspective.
    fail_check();

    const int harvested = harvest_corpses(you);
    dprf("Harvested: %d", harvested);

    if (!harvested)
    {
        if (harvest_corpses(you, true, true))
            mpr("It would be a sin to defile those corpses!");
        else
            canned_msg(MSG_NOTHING_HAPPENS);
        return SPRET_SUCCESS; // still takes a turn, etc
    }

    if (you.attribute[ATTR_BONE_ARMOUR] <= 0)
        mpr("The bodies of the dead rush to embrace you!");
    else
        mpr("Your shell of carrion and bone grows thicker.");

    if (you.permabuffs[MUT_OZOCUBUS_ARMOUR])
        remove_ice_armour();

    // value of ATTR_BONE_ARMOUR will be sqrt(9*harvested), rounded randomly
    int squared = sqr(you.attribute[ATTR_BONE_ARMOUR]) + 9 * harvested;
    you.attribute[ATTR_BONE_ARMOUR] = sqrt(squared) + random_real();
    you.redraw_armour_class = true;

    return SPRET_SUCCESS;
}

spret_type repulsion(int pow, bool fail)
{
    fail_check();
    if(!you.permabuffs[MUT_REPEL_MISSILES])
    {
        // If the player already has Deflect Missiles, abort
        if(you.permabuffs[MUT_DEFLECT_MISSILES])
        {
            mpr("You are already deflecting missiles.");
            return SPRET_ABORT;
        }

        if(spell_add_permabuff(SPELL_REPEL_MISSILES, 2))
        {
            return SPRET_SUCCESS;
        }
        else
            return SPRET_ABORT;
    }
    else
    {
        // Don't have the player pay MP to remove their permabuff
        spell_remove_permabuff(SPELL_REPEL_MISSILES, 2);
        return SPRET_ABORT;
    }
}

spret_type deflection(int pow, bool fail)
{
    fail_check();
    if(!you.permabuffs[MUT_DEFLECT_MISSILES])
    {
        if(spell_add_permabuff(SPELL_DEFLECT_MISSILES, 6))
        {
            // If the player already has Repel Missiles, turn it off
            if(you.permabuffs[MUT_REPEL_MISSILES])
            {
                spell_remove_permabuff(SPELL_REPEL_MISSILES, 2);
            }

            return SPRET_SUCCESS;
        }
        else
            return SPRET_ABORT;
    }
    else
    {
        // Don't have the player pay MP to remove their permabuff
        spell_remove_permabuff(SPELL_DEFLECT_MISSILES, 6);
        return SPRET_ABORT;
    }
}

spret_type cast_regen(int pow, bool fail)
{
    fail_check();
    if(!you.permabuffs[MUT_REGEN_SPELL])
    {
        if(spell_add_permabuff(SPELL_REGENERATION, 3))
        {
            return SPRET_SUCCESS;
        }
        else
            return SPRET_ABORT;
    }
    else
    {
        // Don't have the player pay MP to remove their permabuff
        spell_remove_permabuff(SPELL_REGENERATION, 3);
        return SPRET_ABORT;
    }
}

spret_type cast_revivification(int pow, bool fail)
{
    fail_check();
    mpr("Your body is healed in an amazingly painful way.");

    const int loss = 6 + binomial(9, 8, pow);
    dec_max_hp(loss * you.hp_max / 100);
    set_hp(you.hp_max);

    if (you.duration[DUR_DEATHS_DOOR])
    {
        mprf(MSGCH_DURATION, "Your life is in your own hands once again.");
        // XXX: better cause name?
        paralyse_player("Death's Door abortion");
        you.duration[DUR_DEATHS_DOOR] = 0;
    }
    return SPRET_SUCCESS;
}

spret_type cast_swiftness(int power, bool fail)
{
    fail_check();

    if (you.in_liquid())
    {
        // Hint that the player won't be faster until they leave the liquid.
        mprf("The %s foams!", you.in_water() ? "water"
                            : you.in_lava()  ? "lava"
                                             : "liquid ground");
    }

    you.set_duration(DUR_SWIFTNESS, 12 + random2(power)/2, 30,
                     "You feel quick.");
    you.attribute[ATTR_SWIFTNESS] = you.duration[DUR_SWIFTNESS];

    return SPRET_SUCCESS;
}

int cast_selective_amnesia(const string &pre_msg)
{
    if (you.spell_no == 0)
    {
        canned_msg(MSG_NO_SPELLS);
        return 0;
    }

    int keyin = 0;
    spell_type spell;
    int slot;

    // Pick a spell to forget.
    keyin = list_spells(false, false, false, "Forget which spell?");
    redraw_screen();

    if (isaalpha(keyin))
    {
        spell = get_spell_by_letter(keyin);
        slot = get_spell_slot_by_letter(keyin);

        if (spell != SPELL_NO_SPELL)
        {
            if (!pre_msg.empty())
                mpr(pre_msg);
            del_spell_from_memory_by_slot(slot);
            return 1;
        }
    }

    canned_msg(MSG_OK);
    return -1;
}

spret_type cast_infusion(int pow, bool fail)
{
    fail_check();
    if (!you.permabuffs[MUT_INFUSION])
    {
        if(spell_add_permabuff(SPELL_INFUSION, 1))
        {
            return SPRET_SUCCESS;
        }
        else
            return SPRET_ABORT;
    }
    else
    {
        // Don't have the player pay MP to remove their permabuff
        spell_remove_permabuff(SPELL_INFUSION, 1);
        return SPRET_ABORT;
    }
}

spret_type cast_song_of_slaying(int pow, bool fail)
{
    fail_check();

    if(!you.permabuffs[MUT_SONG_OF_SLAYING])
    {
        if(spell_add_permabuff(SPELL_SONG_OF_SLAYING, 2))
        {
            you.props[SONG_OF_SLAYING_KEY] = 0;
            return SPRET_SUCCESS;
        }
        else
        {
            return SPRET_ABORT;
        }
    }
    else
    {
        spell_remove_permabuff(SPELL_SONG_OF_SLAYING, 2);
        return SPRET_ABORT;
    }
}

spret_type cast_silence(int pow, bool fail)
{
    fail_check();
    mpr("A profound silence engulfs you.");

    you.increase_duration(DUR_SILENCE, 20 + pow/4 + random2avg(pow/2, 2), 100);
    invalidate_agrid(true);

    if (you.beheld())
        you.update_beholders();

    learned_something_new(HINT_YOU_SILENCE);
    return SPRET_SUCCESS;
}

spret_type cast_liquefaction(int pow, bool fail)
{
    fail_check();
    flash_view_delay(UA_PLAYER, BROWN, 80);
    flash_view_delay(UA_PLAYER, YELLOW, 80);
    flash_view_delay(UA_PLAYER, BROWN, 140);

    mpr("The ground around you becomes liquefied!");

    you.increase_duration(DUR_LIQUEFYING, 10 + random2avg(pow, 2), 100);
    invalidate_agrid(true);
    return SPRET_SUCCESS;
}

spret_type cast_shroud_of_golubria(int pow, bool fail)
{
    fail_check();
    if (you.duration[DUR_SHROUD_OF_GOLUBRIA])
        mpr("You renew your shroud.");
    else
        mpr("Space distorts slightly along a thin shroud covering your body.");

    you.increase_duration(DUR_SHROUD_OF_GOLUBRIA, 7 + roll_dice(2, pow), 50);
    return SPRET_SUCCESS;
}

spret_type cast_transform(int pow, transformation which_trans, bool fail)
{
    if (!transform(pow, which_trans, false, true)
        || !check_form_stat_safety(which_trans))
    {
        return SPRET_ABORT;
    }

    fail_check();
    transform(pow, which_trans);
    return SPRET_SUCCESS;
}

spret_type cast_blade_of_disaster(int pow, bool fail)
{
    fail_check();

    mpr("You call disaster forth from your blade!");
        you.set_duration(DUR_BLADE_OF_DISASTER, 5);
        return SPRET_SUCCESS;
}

spret_type cast_time_stop(int pow, bool fail)
{
    fail_check();
    
    mpr("You bring the flow of time to a stop.");
        //You'll lose 10 auts to casting Time Stop, so it's really 10+sp/5 free auts
        you.attribute[ATTR_TIME_STOP] = 20+div_rand_round(pow, 5);
        return SPRET_SUCCESS;
    
    //stasis iterator here
    //Patashu: There's a small possibility for an exploit where you time stop,
    //hit something then swap floors before time stop runs out. The stored damage
    //and momentum never get unleashed! Could be fixed by adding similar code to
    //run whenever you transition to a new level.
    for (monster_iterator mi; mi; ++mi)
    {
        if (mi->alive())
        {
            mi->props[STASIS_DAM].get_int() = 0;
            mi->props[STASIS_VX].get_float() = 0;
            mi->props[STASIS_VY].get_float() = 0;
        }
    }
}

static void stasis_launch(monster *mon, int dam, float vx, float vy);

void stasis_launch(monster *mon, int dam, float vx, float vy)
{
    coord_def destination = coord_def(mon->pos().x + round(vx*8), mon->pos().y + round(vy*8));
    unsigned int distance = max(0, (int)floor(log(mon->props[STASIS_DAM].get_float())/log(2.5)));
    const coord_def oldpos = mon->pos();
    coord_def newpos = oldpos;
    
    bolt beam;
    beam.range           = INFINITE_DISTANCE;
    beam.hit             = AUTOMATIC_HIT;
    beam.pierce          = true;
    beam.affects_nothing = true;
    beam.source          = mon->pos();
    beam.target          = destination;
    beam.aimed_at_spot   = false;
    beam.is_tracer       = true;
    beam.choose_ray();
    if (beam.ray.r.dir.x == 0 && beam.ray.r.dir.y == 0) return;
    
    for (unsigned int j = 0; j < distance; ++j)
    {
        beam.ray.advance();
        newpos = beam.ray.pos();
        if (newpos == mon->pos()
            || cell_is_solid(newpos)
            || actor_at(newpos)
            || !mon->can_pass_through(newpos)
            || !mon->is_habitable(newpos))
        {
            break;
        }
        mon->move_to_pos(newpos);  
    }
    
    if (newpos == oldpos)
        return;
    
    if (you.can_see(*mon))
    {
        mprf("%s %s knocked back by the momentum.",
                 mon->name(DESC_THE).c_str(),
                 mon->conj_verb("are").c_str());
    }
    
    if (mon->pos() != newpos)
        mon->collide(newpos, &you, 17);
        
    mon->apply_location_effects(oldpos, KILL_YOU,
                                actor_to_death_source(&you));
}

void end_time_stop(bool due_to_teleport)
{
    if (due_to_teleport)
    {
        if (you.attribute[ATTR_TIME_STOP] > 0)
        {
            mpr("Being yanked through space has disrupted your control over time.");
        }
        else
        {
            return;
        }
    }
    you.attribute[ATTR_TIME_STOP] = 0;
    you.increase_duration(DUR_EXHAUSTED, 12 + random2(5));
    mpr("Time begins to flow once more.");
    
    //stasis iterator here
    
    bool launched_a_monster = false;
    
    for (monster_iterator mi; mi; ++mi)
    {
        if (mi->alive() && !mons_class_is_stationary(mi->type))
        {
            int dam = mi->props[STASIS_DAM].get_int();
            if (dam >= 1)
            {
                if (!launched_a_monster)
                {
                    mpr("The stored up momentum is unleashed!");
                    launched_a_monster = true;
                }
                
                float vx = mi->props[STASIS_VX].get_float();
                float vy = mi->props[STASIS_VY].get_float();
                stasis_launch(*mi, dam, vx, vy);
            }
        }
        
        mi->props[STASIS_DAM].get_int() = 0;
        mi->props[STASIS_VX].get_float() = 0;
        mi->props[STASIS_VY].get_float() = 0;
    }
}

static mutation_type _permabuff_to_mut(spell_type spell)
{
    switch (spell)
    {
        case SPELL_BEASTLY_APPENDAGE:
        case SPELL_SPIDER_FORM:
        case SPELL_ICE_FORM:
        case SPELL_STATUE_FORM:
        case SPELL_BLADE_HANDS:
        case SPELL_DRAGON_FORM:
        case SPELL_HYDRA_FORM:
        case SPELL_NECROMUTATION:
            return MUT_TRANSFORMATION;
        case SPELL_RING_OF_FLAMES:
            return MUT_RING_OF_FLAMES;
        case SPELL_REGENERATION:
            return MUT_REGEN_SPELL;
        case SPELL_SPECTRAL_WEAPON:
            return MUT_SPECTRAL_WEAPON;
        case SPELL_INFUSION:
            return MUT_INFUSION;
        case SPELL_EXCRUCIATING_WOUNDS:
            return MUT_EXCRUCIATING_WOUNDS;
        case SPELL_OZOCUBUS_ARMOUR:
            return MUT_OZOCUBUS_ARMOUR;
        case SPELL_BATTLESPHERE:
            return MUT_BATTLESPHERE;
        case SPELL_SONG_OF_SLAYING:
            return MUT_SONG_OF_SLAYING;
        case SPELL_DEFLECT_MISSILES:
            return MUT_DEFLECT_MISSILES;
        case SPELL_REPEL_MISSILES:
            return MUT_REPEL_MISSILES;
        default:
            return MUT_NON_MUTATION; //Failed to find permabuff, fail out
    }
}

// Attempt to add a permabuff; return false if it didn't work
bool spell_add_permabuff(spell_type spell, int reserve_amount)
{
    if (!reserve_mp(reserve_amount))
    {
        mpr("You don't have enough energy to reserve that spell!");
        return false;
    }
    else
    {
        // Add permabuff to player's permabuffs here
        mutation_type mutat = _permabuff_to_mut(spell);
        if (mutat == MUT_NON_MUTATION)
        {
            mpr("DEBUG: No mutation found that matches spell, I fucked up.");
            return false;
        }
        else
        {
            mutate(mutat, "adding permabuff", true, false, false,
            false, MUTCLASS_NORMAL, true);
            return true;
        }
    }
}

// Remove the selected spell from permabuffs
void spell_remove_permabuff(spell_type spell, int release_amount)
{
    unreserve_mp(release_amount);
    // Remove permabuff from player's permabuffs here
    mutation_type mutat = _permabuff_to_mut(spell);
    if (mutat == MUT_NON_MUTATION)
    {
        mpr("DEBUG: No mutation found that matches spell, I fucked up.");
    }
    else
    {
        mutate(mutat, "removing permabuff");
    }
}

// Drop all permabuffs (comes from max MP/EP change or armour equipping)
void spell_drop_permabuffs()
{
    // Only go through this if the player has any permabuffs (to avoid message annoyance/doubled messages)
    if(you.mp_max_adj_temp_base != 0)
    {
        // If transformed, drop form
        if (you.form != transformation::none)
        {
            untransform(true);
        }
        // Special case for weapon brands
        if (you.permabuffs[MUT_EXCRUCIATING_WOUNDS])
        {
            item_def& weapon = *you.weapon();
            end_weapon_brand(weapon, true);
        }
        if (you.permabuffs[MUT_SPECTRAL_WEAPON])
        {
            monster* old_spectral = find_spectral_weapon(&you);
            if(old_spectral)
                end_spectral_weapon(old_spectral, false);
        }
        if (you.permabuffs[MUT_BATTLESPHERE])
        {
            monster* old_sphere = find_battlesphere(&you);
            if(old_sphere)
                end_battlesphere(old_sphere, false);
        }
        // Unreserve all MP/EP
        unreserve_mp(-you.mp_max_adj_temp_base);
        // Remove all permabuffs from player's permabuffs here
        for (int i=0; i < NUM_MUTATIONS; i++)
        {
            while (you.has_permabuffs(static_cast<mutation_type>(i)))
            {
                mutate(static_cast<mutation_type>(i), "dropping permabuffs");
            }
        }

        mpr("You lose control of your reserved spells!");
    }
}
