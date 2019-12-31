/**
 * @file
 * @brief Translation filters.
 **/

#include "AppHdr.h"

#include "lang-fake.h"

#include <unordered_set>

#include "libutil.h"
#include "options.h"
#include "random.h"
#include "stringutil.h"
#include "unicode.h"

#define UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define LOWER "abcdefghijklmnopqrstuvwxyz"
#define LETTERS UPPER LOWER

// Any resemblance to the speech of a particular character from Rich Burlew's
// work is entirely accidental. Suggestion that this table has been produced
// by transcribing the whole corpus of that character's speech and making it
// produce the same output from regular English must be dismissed as a rumour.

// For Perl version, "apt-get install filters".
static const char* dwarven[][4] =
{
  {"^yes$","aye"},            {"there","thar"},             {"eir$","ar"},
  {"about","aboot"},          {"^he$","'e"},                {"them","'em"},
  {"^him","'im"},             {"out of$","outta"},          {"of course","'course"},
  {"^of$","o'"},              {"^and$","an'"},              {"to$","ta"},
  {"tog","tag"},              {"that","tha"},               {"the","tha"},
  {"wouldn't","wouldn'ta"},   {"cannot","cannae"},          {"can't","cannae"},
  {"don't","dinnae"},         {"'re$","r"},                 {"for$","fer"},
  {"ver$","'er"},             {"ber$","b'r"},               {"every$","ev'ry"},
  {"^is$","be"},
  {"en$","'n"},               {"^if$","if'n"},              {"enl","'nl"},
  {"eng","'ng"},              {"ing","in'"},                {"ment","mn't"},
  {"^es","'s"},               {"^ex","'s"},                 {"^not$","na"},
  {"^no$","nay"},             {"n't have","n'tve"},
  {"^are$","be"},             {"have","haf"},               {"abl","'bl"},
  {"^you$","ye"},             {"^your","yer"},              {"^you'","ye'"},
  {"noth","nuth"},            {"^this$","'tis"},            {"^here","'ere"},
  {"doesn't","don't"},        {"at a$","atta"},             {"ith$","it'"},
  {"ered$","'red"},           {"into$","inta"},             {"^before","'fore"},
  {"wit' '","wit '"},         {"wit' t","wit t"},           {"wit' w","wit w"},
  {"wit' y","wit y"},         {"get a","git a"},            {"ally$","'lly"},
  {"^my","me"},               {"^i think$","methinks"},     {"nay w","na w"},
  {"^one$","'un"},            {"^'un a","one a"},           {"at ta$","atta"},
  {"ot ta$","otta"},          {"^isn't$","ain't"},          {"^so th","s'th"},
  {"ned$","n'd"},             {"^because","'cause"},
  {0}
};

// Resemblances to the manner of speech of Jägermonsters from "Girl Genius" by
// Phil and Kaja Foglio are... hrm dammit, using the proper name gives it out.
// And s/dammit/gotterdammerung/.
static const char* jager[][4] =
{
  {"this","dis"},
  {"that","dat"},
  {"they","dey"},
  {"their","der"},
  {"there","dere"},
  {"these","dese"},
  {"those","doze"},
  {"the$","de"},
  {"then","den"},
  {"them","dem"},
  {"yes","yah"},
  {"you","hyu"},
  {"hyur","you"},
  {"have","hef"},
  {"wh","v"},
  {"^w","v", 0, " -"}, // "w - a spear" (inventory letters)
  {"w","v", "o", " -"},
  {"^ha","he"},
  {"ha","he", "c"},
  {"any","henny"},
  {"smart","schmot"},
  {"n't","n'"},
  {"ain'","hain't"},
  {"am not","hain't"},
  {"isn'","hain't"},
  {"is not","hain't"},
  {"aren'","hain't"},
  {"are not","haint't"},
  {"good","goot"},
  {"dad","papa"},
  {"stuff","stoff"},
  {"going to","gunna"},
  {"going","gun"},
  {"people","pipple"},
  {"little","liddle"},
  {"love","luv"},
  {"nice","nize"},
  {"bug","bog"},
  {"let me","lemme"},
  {"give me","gimmee"},
  {"castle","kestle"},
  {"clank","klenk"},
  {"better","betta"},
  {"darling","dollink"},
  {"some","zum"},
  {"ouble","ubbel"},
  {"bble","bbel"},
  {"^ex","hex"},
  {"exc","eks"},
  {"ex","eks"},
  {"accident","exident"},
  {"once","vunce"},
  {"id$","eed"},
  {"^one","vun"},
  {"kiss","kees"},
  {"^c","k", 0, "h"},
  {"am$","em"},
  {"^th","t"},
  {"th","dd", "!" LETTERS, "!" LETTERS},
  {"unix","yoonix"},
  {"^un","on"},
  {"^sh","sch", 0, "e"},
  {"^sl","schl"},
  {"^sm","schm"},
  {"^sn","schn"},
  {"^sp","schp"},
  {"^st","scht"},
  {"of","uf"},
  {"qu","qv"},
  {"ing","ink"},
  {"irl","url"},
  {"ish","eesh"},
  {"^i","hy", 0, ")" LETTERS},
  {"ve$","ff", "!" LETTERS},
  {"ect$","eck"},
  {"and","und"},
  {"^all","hall"},
  {"mister","herr"},
  {"mr.","herr"},
  {"^is$","iz"},
  {"god damn it","gotterdammerung"},
  {"goddammit","gotterdammerung"},
  {"goddamnit","gotterdammerung"},
  {"damn it","gotterdammerung"},
  {"goddam","gotterdammerung"},
  {"god damn","gotterdammerung"},
  {"god damn it","gotterdammerung"},
  {"damn","dem"},
  {"god","gott"},
  {"okay","hokay"},
  {"^ok$","hokay"},
  {"just","chust"},
  {0}
};

static const char* runes[][4] =
{
  {"ae",  "ᛇ"},
  {"a",   "ᚨ"},
  {"b",   "ᛒ"},
  {"c",   "ᚲ"},
  {"d",   "ᛞ"},
  {"e",   "ᛖ"},
  {"f",   "ᚠ"},
  {"g",   "ᚷ"},
  {"h",   "ᚺ"},
  {"i",   "ᛁ"},
  {"j",   "ᛃ"},
  {"k",   "ᚲ"},
  {"l",   "ᛚ"},
  {"m",   "ᛗ"},
  {"ng",  "ᛜ"},
  {"n",   "ᚾ"},
  {"o",   "ᛟ"},
  {"ph",  "ᚠ"},
  {"p",   "ᛈ"},
  {"q",   "ᛩ"},
  {"r",   "ᚱ"},
  {"s",   "ᛊ"},
  {"th",  "ᚦ"},
  {"t",   "ᛏ"},
  {"u",   "ᚢ"},
  {"v",   "ᚡ"},
  {"w",   "ᚹ"},
  {"x",   "ᚲᛊ"},
  {"y",   "ᚤ"}, // Anglo-Saxon, sometimes medieval
  {"z",   "ᛉ"},
  {0}
};

static const char* grunt[][4] =
{
  {"^killed$", "annihilated"},
  {"^kill$", "annihilate"},
  {"!", "..."},
  {".", "!", 0, LETTERS "0123456789"},
  {0}
};

   static const char* crab[][4] =
{
    // MONSTERS AND RACES
    {"^orc$","shrimp"},
    {"^goblin","goblin crab"},
    {"urug$","megaprawn"},
    {"kobold","crabold"},
    {"hobgoblin","crabgoblin"},
    {"^elf$","fiddler crab"},
    {"gnoll","doggy crab"},
    {"^ogre","fat crab"},
    {"crazy","crabby"},
    {"fungus","algae"},
    {"^bush","kelp"},
    {"toadstool","pleasing fungus"},
    {"wandering mushroom","unpleased fungus"},
    {"plant","fern"},
    {"thorn hunter","crab hunter"},
    {"briar patch","sentient kelp"},
    {"mangrove","crabgrove"},
    {"basilisk","crabalisk"},
    {"^rat$","clam"},
    {"quokka","barnacle"},
    {"porcupine","porcupine shrimp"},
    {"anaconda","crabaconda"},
    {"black mamba","crab mamma"},
    {"butcher","pincher"},
    {"tengu","flying fishman"},
    {"^troll","squidman"},
    {"abomination","crabomination"},
    {"^lich$","necrocrab"},
    {"eidolon","crabolon"},
    {"wraith","scarycrab"},
    {"ghost","spookycrab"},
    {"minotaur","crabacow"},
    {"demigod","demicrab"},
    {"formicid","antcrab"},
    {"felid","catcrab"},
    {"gargoyle","statuecrab"},
    {"halfling","halfcrab"},
    {"human","crabman"},
    {"naga","snakeman"},
    {"^bear$","bear-crab"},
    {"trapdoor spider","hermit crab"},
    {"merfolk","crab"},
    {"spriggan","sea monkey"},
    {"^ant$","lobster"},
    {"scarab","scare-crab"},
    {"spark wasp","zappy crab"},
    {"melial","axe shrimp"},
    {"^bee$","brine shrimp"},
    {"mosquito","leech"},
    {"giant leech","remorah"},
    {"butterfly","fluttercrab"},
    {"^moth$","flappycrab"},
    {"spider","8-legged crab"},
    {"redback","8-legged redbacked crab"},
    {"tarantella","8-legged dancy crab"},
    {"cockroach","crabroach"},
    {"crawler","scuttler"},
    {"scorpion","scorpion crab"},
    {"entropy weaver","corrodacrab"},
    {"hound","dogfish"},
    {"^wolf","hairy dogfish"},
    {"^warg","fighter dogfish"},
    {"raiju","zappy dogfish"},
    {"centaur","crabataur"},
    {"yaktaur","lobstertaur"},
    {"^faun","slingy crab"},
    {"satyr","piper crab"},
    {"golem","mechanocrab"},
    {"guardian","mechacrab"},
    {"servitor","crabcaster"},
    {"dwarf","dwrab"},
    {"^bat$","batcrab"},
    {"jackal","piranha"},
    {"hydra","headcrab"},
    {"^reaper$","reachycrab"},
    {"^worm","sea worm"},
    {"curse toe","crab doh"},
    {"animal","underwater animal"},
    {"oklob","meltcrab"},
    {"worker","tryhard"},
    // ITEMS
    {"^axe$","cleaving claw"},
    {"^sword$","slashing claw"},
    {"dagger","stabby claw"},
    {"trident","claw"},
    {"halberd","fighting claw"},
    {"glaive","big claw"},
    {"bardiche","fukken big claw"},
    {"demon","demoncrab"},
    {"draconian","crabonian"},
    {"dragon","shark"},
    {"drake","barracuda"},
    {"elemental","crabismental"},
    {"^eye$","eyestalk"},
    {"eyeball","eyestalk"},
    {"^eyes$","eyestalks"},
    {"frost giant","frosty lobsterman"},
    {"hill giant","hilly lobsterman"},
    {"cyclops","one-eyed lobsterman"},
    {"stone giant","stoney lobsterman"},
    {"iron giant","iron lobsterman"},
    {"juggernaut","huggacrab"},
    {"titan","rememberthelobstermans!"},
    {"gold piece","sand dollar"},
    {"gold pieces","sand dollars"},
    {"^bread$","algae"},
    {"^meat$","clam"},
    {"potion","crabjuice"},
    {"^orb$","pearl"},
    {"ignorance","crabbedumb"},
    {"^shield$","carapace"},
    {"^whip","feeler"},
    {"cloak","crabcape"},
    {"amulet","crabdangle"},
    {"manual","crabuhow"},
    {"^gloves$","clawcovers"},
    {"^boots","scuttlers"},
     // GODS
    {"okawaru","crabawaru"},
    {"trog","trog but a crab"},
    {"ashenzari","crabenzari"},
    {"nemelex","crabulex the cardshark"},
    {"cheibriados","crabbyslowbro"},
    {"xom","xom but a crab but also somehow not a crab"},
    {"makhleb","crableg"},
    {"ashenzari","cursincrabby"},
    {"beogh","shrimp jesus"},
    {"kikubaaqudgha","kikubakacrabba"},
    {"fedhas","fedkelp"},
    {"elyvilon","notevilcrab"},
    {"^ru$","thanks crabama"},
    {"lugonu","crabunogo"},
    {"dithmenos","shadowcrab"},
    {"qazlal","crabsoloud"},
    {"uskayaw","undadasea"},
    {"vehumet","acrabumet"},
    {"yredelemnul","reddeadlobster"},
    // UNIQUES
    {"Antaeus","icy mclobsterman"},
    {"fannar","icy crabman"},
    {"duvessa","minmay"},
    {"dowan","crabby mcspells"},
    {"agnes","dungeoness crab"},
    {"asterion","crabnerion"},
    {"boris","orby crabman"},
    {"enchantress","enchantycrab"},
    {"erica","flamy crablady"},
    {"ijyb","itchy crab has a wand"},
    {"jessica","easy crablady"},
    {"eustachio","moustache fancycrab"},
    {"joseph","slingy crabman"},
    {"josephine","decaying crablady"},
    {"khufu","The Fearsome Krab-Fu"},
    {"^mara$","illusocrab the great"},
    {"maurice","craburabbi"},
    {"menkaure","mummy crabman"},
    {"mennas","tport"},
    {"natasha","kitty crablady"},
    {"nessos","hex-vulnerable seahorse nessos"},
    {"pikel","crabbold slavercrab"},
    {"psyche","crazy old crablady"},
    {"^robin$","lil crablin"},
    {"roxanne","anne the rock lobster"},
    {"grinder","tinder crab"},
    {"rupert","he mad crab"},
    {"sigmund","scythy invisocrab"},
    {"sojobo","sudoko crabman"},
    {"sonja","sonja the assassin-crab"},
    {"terence","flaily crab"},
    {"xtahua","jaws"},
    {"polymephus","lobstermephus"},
    {"tiamat","rainbow crab lady"},
    {"arachne","poison crab lady"},
    {"vashnia","slithery shootlady"},
    {"purgy","squidly squidman$",""},
    {"snorg","squorg"},
    {"^grum","crabby dogfisher man"},
    {"the royal jelly","his royal crabbiness"},
    {"nergalle","necroshrimp"},
    {"serpent","lamprey"},
    {"roka","shrimpa"},
    {"nikola","thomas crabma crabbyson"},
    {"blork","blimp"},
    {"josephine","dungeoness crablady"},
    {"monstrosity","cthulhucrab"},
    {"^cob$","crob"},
    {"^wrath$","crabmad"},
    {"klown","clownfish"},
    {"berserk","umadcrab"},
    {"patent armour","greeny crabshell"},
    {"haste","crabfast"},
    {"running","scuttling"},
    {"abomination","crabomination"},
    {"teleport","portacrab"},
    {"^devil","evilcrab"},
    {"tzitzimitl","terrorcrab"},
    // this is here as a reminder that shell is a funny word
    {"^mail$","shell"},
    {"hell sentinel","shell sentinel"},
    {"elephant","shellyphant"},
    {"hellephant","damndatafineshellaphant"},
    {"shapeshifter","shellshaper"},
    {"serpent of hell","sherbert of shell"},
    // GENERAL TEXT REPLACEMENTS
    {"appreciates","clicks claws in appreciation of"},
    {"you die...","your journey is over little crab"},
    {"LOW HITPOINT WARNING","CRAB GONE DIE"},
    {"ouch! that really hurt!","OW CRAB NO LIKE!!!"},
    {"hell knight","red lobsterman"},
    {"^ugly","crabby"},
    {"shout","gurgle"},
    {"electrocution","crabbyzappy"},
    {"electrocute","crabbyzap"},
    {"electrocuted","crabbyzapped"},
    {"^ration$","algae cake"},
    {"javelin","harpoon"},
    {"orcish mines","gump shrimp company"},
    {"shoals","crab heaven"},
    {"elven","fiddler crab"},
    {"transporter","crabpoofer"},
    {"start resting","rest your weary claws"},
    {"staircase","crab tunnel"},
    {"^bark","bubble"},
    {"you climb","you scuttle"},
    {"dodging","scuttling"},
    {"elementalist","crabcaster"},
    {"^mage","castercrab"},
    {"^zot","sebastian"},
    {"unarmed","crab-fu"},
    {"great mace","great mace for a crab"},
    {"spectral weapon","phantom claw"},
    {"corpse","dedcrab"},
    {"^flesh","crabmeat"},
    {"^cave$","crabhole"},
    {"ziggurat","hubrishut"},
    // preserve mistakes
    {"controlled","consquidmaned"},
    {"vault","crabbank"},
    // yes I am aware this may make hell sentinels shell shelltenals
    {"sentinel","shelltenal"},
    {"depths","trench"},
    {"volcano","hydrothermal vent"},
    {"^tomb$","Davy Jones' Locker"},
    {"robe of misfortune","why would you even put this on, girlcrab?"},
    // this will probably have unintended consequences
    {"^flight","crabwings"},
    {"heal wounds","saveacrab"},
    {0}
};



static void _replace_cap_variants(string &str,
                                  string a,
                                  string b,
                                  const char* not_after = 0,
                                  const char* not_before = 0)
{
    ASSERT(!a.empty());
    if (a[0] == '^')
    {
        a.erase(0, 1);
        ASSERT(!not_after);
        not_after = LETTERS;
    }

    ASSERT(!a.empty());
    if (a[a.length() - 1] == '$')
    {
        a.erase(a.length() - 1);
        ASSERT(!not_before);
        not_before = LETTERS;
    }

    ASSERT(!a.empty());

    bool yes_after = false;
    if (not_after && *not_after == '!')
        yes_after = true, not_after++;

    bool yes_before = false;
    if (not_before && *not_before == '!')
        yes_before = true, not_before++;

    for (int captype = 0; captype < 3; captype++)
    {
        string A;
        switch (captype)
        {
        case 0: A = lowercase(a); break;
        case 1: A = uppercase_first(a); break;
        case 2: A = uppercase(a); break;
        default: die("lolwut?");
        }

        size_t pos = 0;
        while ((pos = str.find(A, pos)) != string::npos)
        {
            if (not_after && pos > 0
                && (yes_after == !strchr(not_after, str[pos - 1])))
            {
                pos++;
                continue;
            }

            size_t epos = pos + A.length();
            if (not_before && epos < str.length()
                && (yes_before == !strchr(not_before, str[epos])))
            {
                pos++;
                continue;
            }

            string B;
            switch (captype)
            {
            case 0: B = lowercase(b); break;
            case 1: B = uppercase_first(b); break;
            case 2: B = uppercase(b); break;
            default: die("lolwut?");
            }

            str.erase(pos, A.length());
            str.insert(pos, B);
            pos += B.length();
        }
    }
}

static void _german(string &txt)
{
    /*
    The European Commission has just announced an agreement whereby English will
    be the official language of the European Union rather than German, which was
    the other possibility.

    As part of the negotiations with the Germans, the British Government
    conceded that English spelling had some room for improvement and has
    accepted a 5-year phase-in plan that would become known as "Euro-English".

    In the first year, "s" will replace the soft "c". Sertainly, this will make
    the sivil servants jump with joy.

    The hard "c" will be dropped in favour of "k". This should klear up
    konfusion, and keyboards kan have one less letter.
    */
    for (unsigned int i = 0; i < txt.length(); i++)
        if (txt[i] == 'c' || txt[i] == 'C')
        {
            switch (tolower(txt[i+1]))
            {
            case 'h': case 'z':
                break;
            case 'e': case 'i': case 'y':
                txt[i] = isalower(txt[i]) ? 's' : 'S';
                break;
            default:
                txt[i] = isalower(txt[i]) ? 'k' : 'K';
            }
        }
    /*
    There will be growing publik enthusiasm in the sekond year when the
    troublesome "ph" will be replaced with "f". This will make words like
    fotograf 20% shorter.
    */
    _replace_cap_variants(txt, "ph", "f");
    /*
    In the 3rd year, publik akseptanse of the new spelling kan be expekted to
    reach the stage where more komplikated changes are possible.

    Governments will enkourage the removal of double letters which have always
    ben a deterent to akurate speling.
    */
    for (int i = txt.length() - 2; i > 0; i--)
        if (isalpha(txt[i]) && txt[i] == txt[i + 1])
            txt.erase(i, 1);
    /*
    Also, al wil agre that the horibl mes of the silent "e" in the languag is
    disgrasful and it should go away.
    */
    for (int i = txt.length() - 2; i > 0; i--)
        if (txt[i]=='e' || txt[i]=='E')
        {
            if (i < 3)
                continue;
            if (isaalpha(txt[i + 1]))
                continue;
            if (txt[i-1]=='e' || txt[i-1]=='E')
                continue;
            if (!isaalpha(txt[i-1])
              || !isaalpha(txt[i-2])
              || !isaalpha(txt[i-3]))
            {
                continue;
            }
            txt.erase(i, 1);
        }
    /*
    By the 4th yer people wil be reseptiv to steps such as replasing "th" with
    "z" and "w" with "v".
    */
    _replace_cap_variants(txt, "th", "z");
    _replace_cap_variants(txt, "w", "v");
    /*
    During ze fifz yer, ze unesesary "o" kan be dropd from vords containing "ou"
    and after ziz fifz yer, ve vil hav a reil sensibl riten styl.
    */
    _replace_cap_variants(txt, "ou", "u");
    /*
    Zer vil be no mor trubl or difikultis and evrivun vil find it ezi tu
    understand ech oza. Ze drem of a united urop vil finali kum tru.

    Und efter ze fifz yer, ve vil al be speking German like zey vunted in ze
    forst plas.
    */
    // TODO :p
}

static const char* german[][4] =
{
  {"sh",    "sch"},
  {"^is$",  "ist"},
  {"an$",   "ann"},
  {"^and$", "und"},
  {"^one$", "ein"},
  {0}
};

static void _wide(string &txt)
{
    string out;

    for (char ch : txt)
    {
        if (ch == ' ')
            out += "　"; // U+3000 rather than U+FF00
        else if (ch > 32 && ch < 127)
        {
            char buf[4];
            int r = wctoutf8(buf, ch + 0xFF00 - 32);
            for (int j = 0; j < r; j++)
                out.push_back(buf[j]);
        }
        else
            out.push_back(ch);
    }

    txt = out;
}

static void _grunt(string &txt)
{
    static const char* exact_grunt[][2] =
    {
        {"battlesphere", "BATTLESPHERE"},
        {"Battlesphere", "BATTLESPHERE"},
        {"battlemage", "BATTLEMAGE"},
        {"Battlemage", "BATTLEMAGE"},
        {"Battle Magician", "BATTLEMAGE"},
        {"battleaxe", "BATTLEAXE"},
        {"book of Battle", "BATTLEBOOK"},
        {"Battlelust", "BATTLELUST"},
        {"Kill them all", "RIP AND TEAR"},
        {"accepts your kill", "roars: ANNIHILATED"},
        {"appreciates your killing of a heretic priest",
         "smash puny heretic"},
        {"appreciates your killing of a magic user",
         "smash puny caster"},
        {"appreciates your killing of a holy being",
         "smash puny angel"},
        {"is honoured by your kill", "screams: ANNIHILATED"},
        {"You pass out from exhaustion.", "POWER NAP!!!"},
        {"You die...", "rip"},
        {"LOW HITPOINT WARNING", "don't die"},
        {"Ouch! That really hurt!", "DANG"},
        {0}
    };

    const char* (*repl)[2] = exact_grunt;
    for (; **repl; repl++)
    {
        size_t pos = 0;
        string a = (*repl)[0];
        string b = (*repl)[1];
        while ((pos = txt.find(a, pos)) != string::npos)
        {
            txt.erase(pos, a.length());
            txt.insert(pos, b);
            pos += b.length();
        }
    }
}

/**
 * Greedily look for a token consisting of only letters, starting at the
 * given index into the given string.
 *
 * @return      The index into the string at which the token ends.
 *              For example, if passed ("bird bee", 5), return 8.
 *              If passed ("bird bee, 4"), return 4 (empty token)
 */
static size_t _token_starting(const string &str, size_t start)
{
    size_t end;
    for (end = start; end < str.size() && isaalpha(str[end]); ++end)
        ;
    return end;
}

/**
 * Is the given string token too boring to be replaced with 'butt'?
 *
 * @param token     The string in question. Should be lowercased.
 * @return          Whether the token should be skipped in buttification.
 */
static bool _too_boring_to_butt(const string &token)
{
    static const unordered_set<string> boring_words = {
        "a", "an", "the", // articles
        "and", "or", "of", "at", "on", "in", "if", "into", "to", "with", // etc
        // , "but" <- this is actually very funny to replace.
    };
    return boring_words.count(token);
}

/**
 * Return an appropriate string to replace the given token with.
 *
 * @param token     The token to be replaced.
 * @return          Presumably, some variant on 'butt'.
 */
static string _replacement_butt(const string &token)
{
    string butt = "butt";
    char plural = 's';
    const char *ly = "-ly";
    if (isupper(token[0]))
    {
        // All caps and not a single-letter word?
        if (token.size() > 1 && uppercase_string(token) == token)
        {
            uppercase(butt); // FOO -> BUTT
            plural = 'S';
            ly = "-LY";
        }
        else
            butt = uppercase_first(butt); // Foo -> Butt
    }

    const string lctok = lowercase_string(token);
    size_t size = lctok.size();

    // Possessives, plurals, and adverbs (and a few adjectives like "holy",
    // "ugly").
    if (ends_with(lctok, "'s") || ends_with(lctok, "s'")
        || lctok == "your" || lctok == "my" || lctok == "its" || lctok == "his"
        || lctok == "our" || lctok == "their") // "her" is ambiguous :(
    {
        butt += '\''; butt += plural; // your -> butt's, Sigmund's -> Butt's
    }
    else if (lctok[size - 1] == 's')
        butt += plural; // Foos -> Butts, FOOS -> BUTTS
    else if (size > 3 && lctok.substr(size - 2, 2) == "ly")
        butt += ly; // carefully -> butt-ly, THUNDEROUSLY -> BUTT-LY

    return butt;
}

/**
 * Reference https://code.google.com/p/buttbot/ for vision statement & details.
 *
 * @param str[in,out]   The string to be improved.
 * @param odds          The raw % chance for a given word to be buttified. Will
 *                      Will be bounded between 0-100 (inclusive) if specified;
 *                      -1 indicates that the default odds (5%) should be used.
 */
static void _butt(string &str, int odds)
{
    // iter along the string, tokenizing & potentially replacing as we go.
    for (size_t start = 0; start < str.size(); ++start)
    {
        const size_t end = _token_starting(str, start);

        if (end == start) // empty token (non-alpha index)
            continue;

        const int real_odds = odds == -1 ? 5 : max(0, min(100, odds));

        const string token = str.substr(start, end - start);
        // butt sparingly.
        if (!x_chance_in_y(real_odds, 100)
            || _too_boring_to_butt(lowercase_string(token)))
        {
            start = end; // will increment again, past the token-ending char
            continue;
        }

        // here's where the magic happens!!!
        const string butt = _replacement_butt(token);
        str.erase(start, end - start);
        str.insert(start, butt);
        start += butt.size(); // will incr again, past the token-ending char
    }
}


void filter_lang(string &str)
{
    for (auto fake_lang : Options.fake_langs)
    {
        const char* (*repl)[4];

        switch (fake_lang.lang_id)
        {
            case flang_t::dwarven:
                repl = dwarven;
                break;
            case flang_t::jagerkin:
                repl = jager;
                break;
            case flang_t::kraut:
                _german(str), repl = german;
                break;
            case flang_t::wide:
                return _wide(str);
            case flang_t::futhark:
                repl = runes;
                break;
            case flang_t::grunt:
                _grunt(str); repl = grunt;
                break;
            case flang_t::butt:
                _butt(str, fake_lang.value);
                continue;
            case flang_t::crab:
                repl = crab;
                break;
            default:
                continue;
        }

        for (; **repl; repl++)
            _replace_cap_variants(str, (*repl)[0], (*repl)[1], (*repl)[2], (*repl)[3]);
    }
}

string filtered_lang(string str)
{
    filter_lang(str);
    return str;
}
