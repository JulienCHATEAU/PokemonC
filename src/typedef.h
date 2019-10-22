#ifndef TYPEDEF_H
#define TYPEDEF_H

#define BAG_SIZE 50
#define ITEM_COUNT 100
typedef enum {
  NO_TYPE = -1,
  NORMAL,   // 0
  FIGHTING, // 1
  FLYING,   // 2
  POISON,   // 3
  GROUND,   // 4
  ROCK,     // 5
  BUG,      // 6
  GHOST,    // 7
  STEEL,    // 8
  FIRE,     // 9
  WATER,    // 10
  GRASS,    // 11
  ELECTRIC, // 12
  PSYCHIC,  // 13
  ICE,      // 14
  DRAGON,   // 15
  DARK,     // 16
  FAIRY,    // 17
} Type;

typedef enum {
  NO_STATENUM = -1,
  ATT, // 1
  DEF, // 0
  SPD, // 2
  ACC, // 3
} StatEnum;

typedef enum {
  NO_AILMENT = -1,
  PARALYSIS, // 0
  SLEEP,     // 2
  FREEZE,    // 3
  BURN,      // 4
  POISONING, // 5
  CONFUSION, // 6
} AilmentEnum;

typedef enum {
  POKEBALL_ID,    // 0
  POTION_ID,      // 1
  CANNE_ID,       // 2
  CISAILLE_ID,    // 3
  PIOCHE_ID,      // 4
  CRISTAL_KEY_ID, // 5
  EMERALD_KEY_ID, // 6
  DIAMOND_KEY_ID, // 7
  X_KEY_ID,       // 8
  SHARP_KEY_ID,   // 9
  KEY_FRAG_ID,    // 10
  TOTAL_SOIN_ID,  // 11
} BagItemId;

typedef struct {
  StatEnum stat_to_change;
  int change_value;
  int accuracy;
  int target;
} Effect;

typedef struct {
  AilmentEnum ailment_enum;
  int accuracy;
} Ailment;

typedef struct {
  int hp;
  int hp_max;
  int att;
  int att_max;
  int def;
  int def_max;
  int spd;
  int spd_max;
} Stats;

typedef struct {
  int name_length;
  char *name;
  int power;
  int accuracy_max;
  int accuracy;
  int pp_max;
  int pp;
  Type type;
  int priority;
  Effect effect;
  Ailment ailment;
  int description_length;
  char *description;
  int id;
} Skill;

typedef struct {
  int name_length;
  char *name;
  int lvl;
  int xp;
  int base_xp;
  Skill skills[4];
  AilmentEnum crt_ailments[2];
  int effect_stage[6];
  Type type[2];
  Stats stats;
  int evo_lvl;
  int evo_name_length;
  char *evo_name;
} Pokemon;

typedef enum {
  IN_BATTLE,       // 0
  OUT_BATTLE,      // 1
  IN_N_OUT_BATTLE, // 2
  NOWHERE,         // 3
} ItemUse;

typedef struct {
  char *name;
  int name_length;
  int id;
  char *description;
  int description_length;
  int count;
  int price;
  ItemUse usable_time; // this bag item is useable : 0 in battle, 1 out battle,
                       // 2 both, 3 nowhere
} BagItem;

typedef struct {
  int x_map;
  int y_map;
  int xy;
} MapSquare;

typedef struct {
  int xy;
  char pos;
  char char_at_pos;
  Pokemon pkmns[6];
  char pseudo[50];
  int pkmn_count;
  int pseudo_length;
  int money;
  BagItem bag[BAG_SIZE];
  int bag_item_count;
  MapSquare item_picked[ITEM_COUNT];
  int item_picked_count;
} Player;

typedef struct {
  MapSquare square;
  int name_length;
  char *name;
  int texts_count;
  int *texts_length;
  char **dialog_texts;
} Pnj;

#endif