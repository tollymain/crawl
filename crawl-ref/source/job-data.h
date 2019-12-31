enum weapon_choice
{
    WCHOICE_NONE,   ///< No weapon choice
    WCHOICE_PLAIN,  ///< Normal weapon choice
    WCHOICE_GOOD,   ///< Chooses from "good" weapons
    WCHOICE_RANGED, ///< Choice of ranged weapon
};

struct job_def
{
    const char* abbrev; ///< Two-letter abbreviation
    const char* name; ///< Long name
    int s, i, d; ///< Starting Str, Dex, and Int
    vector<species_type> recommended_species; ///< Which species are good at it
    /// Guaranteed starting equipment. Uses vault spec syntax, with the plus:,
    /// charges:, q:, and ego: tags supported.
    vector<string> equipment;
    weapon_choice wchoice; ///< how the weapon is chosen, if any
    vector<pair<skill_type, int>> skills; ///< starting skills
};

static const map<job_type, job_def> job_data =
{

{ JOB_ABYSSAL_KNIGHT, {
    "AK", "Abyssal Knight",
    4, 4, 4,
    { SP_HILL_ORC, SP_SPRIGGAN, SP_TROLL, SP_MERFOLK, SP_BASE_DRACONIAN,
      SP_DEMONSPAWN, },
    { "leather armour" },
    WCHOICE_PLAIN,
    { { SK_FIGHTING, 3 }, { SK_ARMOUR, 1 }, { SK_DODGING, 1 },
      { SK_INVOCATIONS, 2 }, { SK_WEAPON, 2 }, },
} },

{ JOB_AIR_ELEMENTALIST, {
    "AE", "Air Elementalist",
    0, 7, 5,
    { SP_HIGH_ELF, SP_DEEP_ELF, SP_TENGU, SP_BASE_DRACONIAN, SP_NAGA,
      SP_VINE_STALKER, SP_FAERIE_DRAGON, },
    { "robe", "book of Air" },
    WCHOICE_NONE,
    { { SK_CONJURATIONS, 1 }, { SK_AIR_MAGIC, 3 }, { SK_SPELLCASTING, 2 },
      { SK_DODGING, 2 }, { SK_STEALTH, 2 }, },
} },

{ JOB_ARCANE_MARKSMAN, {
    "AM", "Arcane Marksman",
    2, 5, 5,
    { SP_HIGH_ELF, SP_DEEP_ELF, SP_KOBOLD, SP_SPRIGGAN, SP_TROLL, SP_CENTAUR, },
    { "robe", "book of Debilitation" },
    WCHOICE_RANGED,
    { { SK_FIGHTING, 1 }, { SK_DODGING, 2 }, { SK_SPELLCASTING, 1 },
      { SK_HEXES, 3 }, { SK_WEAPON, 2 }, },
} },

{ JOB_ARCHAEOLOGIST, {
    "Ac", "Archaeologist",
    4, 4, 4,
    { SP_HUMAN, SP_TENGU, SP_DEEP_ELF, SP_DEEP_DWARF, SP_DEMONSPAWN},
    { "whip", "robe plus:1", "hat plus:1", "dusty tome", "ancient crate","scroll of identify q:4" },
    WCHOICE_NONE,
    { { SK_STEALTH, 1}, { SK_DODGING, 3}, { SK_FIGHTING, 3} },
} },
    
{ JOB_ARTIFICER, {
    "Ar", "Artificer",
    4, 3, 5,
    { SP_DEEP_DWARF, SP_HALFLING, SP_KOBOLD, SP_SPRIGGAN, SP_BASE_DRACONIAN,
      SP_DEMONSPAWN, },
    { "short sword", "leather armour", "wand of flame charges:15",
      "wand of enslavement charges:15", "wand of random effects charges:15" },
    WCHOICE_NONE,
    { { SK_EVOCATIONS, 3 }, { SK_DODGING, 2 }, { SK_FIGHTING, 1 },
      { SK_WEAPON, 1 }, { SK_STEALTH, 1 }, },
} },

{ JOB_ASSASSIN, {
    "As", "Assassin",
    3, 3, 6,
    { SP_TROLL, SP_HALFLING, SP_SPRIGGAN, SP_DEMONSPAWN, SP_VAMPIRE,
      SP_VINE_STALKER, },
    { "dagger plus:2", "blowgun", "robe", "cloak", "needle ego:poisoned q:8",
      "needle ego:curare q:2" },
    WCHOICE_NONE,
    { { SK_FIGHTING, 2 }, { SK_DODGING, 1 }, { SK_STEALTH, 4 },
      { SK_THROWING, 2 }, { SK_WEAPON, 2 }, },
} },

{ JOB_BERSERKER, {
    "Be", "Berserker",
    9, -1, 4,
    { SP_HILL_ORC, SP_HALFLING, SP_OGRE, SP_MERFOLK, SP_MINOTAUR, SP_GARGOYLE,
      SP_DEMONSPAWN, },
    { "animal skin" },
    WCHOICE_PLAIN,
    { { SK_FIGHTING, 3 }, { SK_DODGING, 2 }, { SK_WEAPON, 3 }, },
} },

{ JOB_CHAOS_KNIGHT, {
    "CK", "Chaos Knight",
    4, 4, 4,
    { SP_HILL_ORC, SP_TROLL, SP_CENTAUR, SP_MERFOLK, SP_MINOTAUR,
      SP_BASE_DRACONIAN, SP_DEMONSPAWN, },
    { "leather armour plus:2" },
    WCHOICE_PLAIN,
    { { SK_FIGHTING, 3 }, { SK_ARMOUR, 1 }, { SK_DODGING, 1 },
      { SK_WEAPON, 3 } },
} },

{ JOB_CONJURER, {
    "Cj", "Conjurer",
    0, 7, 5,
    { SP_HIGH_ELF, SP_DEEP_ELF, SP_NAGA, SP_TENGU, SP_BASE_DRACONIAN,
      SP_PROMETHEAN, SP_FAERIE_DRAGON, },
    { "robe", "book of Conjurations" },
    WCHOICE_NONE,
    { { SK_CONJURATIONS, 4 }, { SK_SPELLCASTING, 2 }, { SK_DODGING, 2 },
      { SK_STEALTH, 2 }, },
} },

{ JOB_EARTH_ELEMENTALIST, {
    "EE", "Earth Elementalist",
    0, 7, 5,
    { SP_DEEP_ELF, SP_DEEP_DWARF, SP_SPRIGGAN, SP_GARGOYLE, SP_PROMETHEAN,
      SP_GHOUL, SP_OCTOPODE, SP_FAERIE_DRAGON, },
    { "book of Geomancy", "stone q:30", "robe", },
    WCHOICE_NONE,
    { { SK_TRANSMUTATIONS, 1 }, { SK_EARTH_MAGIC, 3 }, { SK_SPELLCASTING, 2 },
      { SK_DODGING, 2 }, { SK_STEALTH, 2 }, }
} },

{ JOB_ENCHANTER, {
    "En", "Enchanter",
    0, 7, 5,
    { SP_DEEP_ELF, SP_FELID, SP_KOBOLD, SP_SPRIGGAN, SP_NAGA, SP_VAMPIRE, },
    { "dagger plus:1", "robe", "book of Maledictions" },
    WCHOICE_NONE,
    { { SK_WEAPON, 1 }, { SK_HEXES, 3 }, { SK_SPELLCASTING, 2 },
      { SK_DODGING, 2 }, { SK_STEALTH, 3 }, },
} },

{ JOB_FIGHTER, {
    "Fi", "Fighter",
    8, 0, 4,
    { SP_DEEP_DWARF, SP_HILL_ORC, SP_TROLL, SP_MINOTAUR, SP_GARGOYLE,
      SP_CENTAUR, },
    { "scale mail", "shield", "potion of might" },
    WCHOICE_GOOD,
    { { SK_FIGHTING, 3 }, { SK_SHIELDS, 3 }, { SK_ARMOUR, 3 },
      { SK_WEAPON, 2 } },
} },

{ JOB_FIRE_ELEMENTALIST, {
    "FE", "Fire Elementalist",
    0, 7, 5,
    { SP_HIGH_ELF, SP_DEEP_ELF, SP_HILL_ORC, SP_NAGA, SP_TENGU, SP_PROMETHEAN,
      SP_GARGOYLE, SP_FAERIE_DRAGON, },
    { "robe", "book of Flames" },
    WCHOICE_NONE,
    { { SK_CONJURATIONS, 1 }, { SK_FIRE_MAGIC, 3 }, { SK_SPELLCASTING, 2 },
      { SK_DODGING, 2 }, { SK_STEALTH, 2 }, },
} },

{ JOB_GLADIATOR, {
    "Gl", "Gladiator",
    6, 0, 6,
    { SP_DEEP_DWARF, SP_HILL_ORC, SP_MERFOLK, SP_MINOTAUR, SP_GARGOYLE,
      SP_CENTAUR, },
    { "leather armour", "helmet", "throwing net q:3" },
    WCHOICE_GOOD,
    { { SK_FIGHTING, 2 }, { SK_THROWING, 2 }, { SK_DODGING, 3 },
      { SK_WEAPON, 3}, },
} },

{ JOB_HUNTER, {
    "Hu", "Hunter",
    4, 3, 5,
    { SP_HIGH_ELF, SP_HILL_ORC, SP_HALFLING, SP_KOBOLD, SP_OGRE, SP_TROLL,
      SP_CENTAUR, },
    { "short sword", "leather armour" },
    WCHOICE_RANGED,
    { { SK_FIGHTING, 2 }, { SK_DODGING, 2 }, { SK_STEALTH, 1 },
      { SK_WEAPON, 4 }, },
} },

{ JOB_ICE_ELEMENTALIST, {
    "IE", "Ice Elementalist",
    0, 7, 5,
    { SP_HIGH_ELF, SP_DEEP_ELF, SP_MERFOLK, SP_NAGA, SP_BASE_DRACONIAN,
      SP_PROMETHEAN, SP_GARGOYLE, SP_FAERIE_DRAGON, },
    { "robe", "book of Frost" },
    WCHOICE_NONE,
    { { SK_CONJURATIONS, 1 }, { SK_ICE_MAGIC, 3 }, { SK_SPELLCASTING, 2 },
      { SK_DODGING, 2 }, { SK_STEALTH, 2 }, },
} },

{ JOB_SLIME_APOSTLE, {
    "SA", "Slime Apostle",
    4, 4, 4,
    { SP_TROLL, SP_OGRE, SP_FELID, SP_OCTOPODE, SP_HERMIT_CRAB, },
    { "robe", "potion of mutation" },
    WCHOICE_PLAIN,
    { { SK_FIGHTING, 1 }, { SK_WEAPON, 2 }, { SK_DODGING, 3 },
      { SK_STEALTH, 2 }, },
} },

{ JOB_MONK, {
    "Mo", "Monk",
    3, 2, 7,
    { SP_DEEP_DWARF, SP_HILL_ORC, SP_TROLL, SP_CENTAUR, SP_MERFOLK,
      SP_GARGOYLE, SP_DEMONSPAWN, },
    { "robe" },
    WCHOICE_PLAIN,
    { { SK_FIGHTING, 3 }, { SK_WEAPON, 3 }, { SK_DODGING, 3 },
      { SK_STEALTH, 2 }, },
} },

{ JOB_NECROMANCER, {
    "Ne", "Necromancer",
    0, 7, 5,
    { SP_DEEP_ELF, SP_DEEP_DWARF, SP_HILL_ORC, SP_DEMONSPAWN, SP_MUMMY,
      SP_VAMPIRE, SP_FAERIE_DRAGON, },
    { "robe", "book of Necromancy" },
    WCHOICE_NONE,
    { { SK_SPELLCASTING, 2 }, { SK_NECROMANCY, 4 }, { SK_DODGING, 2 },
      { SK_STEALTH, 2 }, },
} },

{ JOB_SKALD, {
    "Sk", "Skald",
    3, 5, 4,
    { SP_HIGH_ELF, SP_HALFLING, SP_CENTAUR, SP_MERFOLK, SP_BASE_DRACONIAN,
      SP_VAMPIRE, },
    { "leather armour", "book of Battle" },
    WCHOICE_PLAIN,
    { { SK_FIGHTING, 2 }, { SK_ARMOUR, 1 }, { SK_DODGING, 2 },
      { SK_SPELLCASTING, 2 }, { SK_CHARMS, 3 }, { SK_WEAPON, 2 }, },
} },

{ JOB_SUMMONER, {
    "Su", "Summoner",
    0, 7, 5,
    { SP_DEEP_ELF, SP_HILL_ORC, SP_VINE_STALKER, SP_MERFOLK, SP_TENGU,
      SP_VAMPIRE, },
    { "robe", "book of Callings" },
    WCHOICE_NONE,
    { { SK_SUMMONINGS, 4 }, { SK_SPELLCASTING, 2 }, { SK_DODGING, 2 },
      { SK_STEALTH, 2 }, },
} },

{ JOB_TRANSMUTER, {
    "Tm", "Transmuter",
    2, 5, 5,
    { SP_NAGA, SP_MERFOLK, SP_BASE_DRACONIAN, SP_PROMETHEAN, SP_DEMONSPAWN,
      SP_TROLL, },
    { "arrow q:12", "robe", "book of Changes" },
    WCHOICE_NONE,
    { { SK_FIGHTING, 1 }, { SK_UNARMED_COMBAT, 3 }, { SK_DODGING, 2 },
      { SK_SPELLCASTING, 2 }, { SK_TRANSMUTATIONS, 2 }, },
} },

{ JOB_VENOM_MAGE, {
    "VM", "Venom Mage",
    0, 7, 5,
    { SP_DEEP_ELF, SP_SPRIGGAN, SP_NAGA, SP_MERFOLK, SP_TENGU, SP_FELID,
      SP_DEMONSPAWN, SP_FAERIE_DRAGON, },
    { "robe", "Young Poisoner's Handbook" },
    WCHOICE_NONE,
    { { SK_CONJURATIONS, 1 }, { SK_POISON_MAGIC, 3 }, { SK_SPELLCASTING, 2 },
      { SK_DODGING, 2 }, { SK_STEALTH, 2 }, },
} },

{ JOB_WANDERER, {
    "Wn", "Wanderer",
    0, 0, 0, // Randomised
    { SP_HILL_ORC, SP_SPRIGGAN, SP_CENTAUR, SP_MERFOLK, SP_BASE_DRACONIAN,
      SP_HUMAN, SP_DEMONSPAWN, },
    { }, // Randomised
    WCHOICE_NONE,
    { }, // Randomised
} },

{ JOB_WARPER, {
    "Wr", "Warper",
    3, 5, 4,
    { SP_HALFLING, SP_HIGH_ELF, SP_DEEP_DWARF, SP_SPRIGGAN, SP_CENTAUR,
      SP_BASE_DRACONIAN, },
    { "leather armour", "book of Spatial Translocations", "scroll of blinking",
      "tomahawk ego:dispersal q:5" },
    WCHOICE_PLAIN,
    { { SK_FIGHTING, 2 }, { SK_ARMOUR, 1 }, { SK_DODGING, 2 },
      { SK_SPELLCASTING, 2 }, { SK_TRANSLOCATIONS, 3 }, { SK_THROWING, 1 },
      { SK_WEAPON, 2 }, },
} },

{ JOB_WIZARD, {
    "Wz", "Wizard",
    -1, 10, 3,
    { SP_HIGH_ELF, SP_DEEP_ELF, SP_NAGA, SP_BASE_DRACONIAN, SP_OCTOPODE,
      SP_HUMAN, SP_MUMMY, SP_FAERIE_DRAGON, },
    { "robe", "hat", "book of Minor Magic" },
    WCHOICE_NONE,
    { { SK_DODGING, 2 }, { SK_STEALTH, 2 }, { SK_SPELLCASTING, 3 },
      { SK_TRANSLOCATIONS, 1 }, { SK_CONJURATIONS, 1 }, { SK_SUMMONINGS, 1 }, },
} },

{ JOB_DEATH_KNIGHT, {
    "DK", "Death Knight",
    5, 3, 4,
    { SP_HUMAN, SP_MINOTAUR, SP_HILL_ORC, SP_DEMONSPAWN, SP_MERFOLK,
      SP_BASE_DRACONIAN, SP_MUMMY, },
    { "leather armour plus:1" },
    WCHOICE_PLAIN,
    { { SK_FIGHTING, 2 }, { SK_ARMOUR, 1 }, { SK_DODGING, 1 },
      { SK_INVOCATIONS, 3 }, { SK_WEAPON, 2 }, },
} },

{ JOB_PRIEST, {
    "Pr", "Priest",
    4, 4, 4,
    { SP_HUMAN, SP_HILL_ORC, SP_FORMICID, SP_HALFLING, SP_KOBOLD,
      SP_SPRIGGAN, SP_BASE_DRACONIAN },
    { "robe" },
    WCHOICE_PLAIN,
    { { SK_FIGHTING, 2 }, { SK_DODGING, 1 }, { SK_INVOCATIONS, 5 }, { SK_WEAPON, 3 }, },
} },

{ JOB_HEALER, {
    "He", "Healer",
    4, 4, 4,
    { SP_DEEP_DWARF, SP_HILL_ORC, SP_NAGA, SP_MINOTAUR, SP_BASE_DRACONIAN,
      SP_GARGOYLE, },
    { "robe plus:1", "potion of curing", "potion of heal wounds" },
    WCHOICE_NONE,
    { { SK_FIGHTING, 1 }, { SK_DODGING, 2 }, { SK_INVOCATIONS, 3 }, },
} },

{ JOB_JESTER, {
    "Jr", "Jester",
    1, 5, 6,
    { SP_HUMAN, SP_GNOLL, SP_TENGU, SP_KOBOLD, SP_SPRIGGAN, SP_OCTOPODE, },
    { "quarterstaff ego:chaos" "robe", "cap", "book of Party Tricks", 
      "pie ego:blinding q:4"},
    WCHOICE_NONE,
    { { SK_DODGING, 2 }, { SK_STAVES, 1 }, { SK_STEALTH, 1 }, { SK_THROWING, 1 },
      { SK_SPELLCASTING, 2 }, { SK_EVOCATIONS, 2 }, },
} },

{ JOB_REAVER, {
    "Re", "Reaver",
    1, 6, 5,
    { SP_HIGH_ELF, SP_BASE_DRACONIAN, SP_TENGU, SP_GARGOYLE, SP_OGRE,},
    { "leather armour", "cloak", "book of Reaving"
        },
    WCHOICE_PLAIN,
    { { SK_DODGING, 2 }, {SK_FIGHTING, 2 }, { SK_WEAPON, 2 }, { SK_SPELLCASTING, 2 },
        {SK_CONJURATIONS, 2 }, {SK_CHARMS, 2 }, },
    } },
    
#if TAG_MAJOR_VERSION == 34
{ JOB_STALKER, {
    "St", "Stalker",
    0, 0, 0,
    { },
    { },
    WCHOICE_NONE,
    { },
} },
#endif
};
