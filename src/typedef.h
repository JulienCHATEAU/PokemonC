#ifndef TYPEDEF_H
#define TYPEDEF_H

#define BAG_SIZE 50

typedef enum {
  NO_TYPE = -1,
  NORMAL,//0
  FIGHTING,//1
  FLYING,//2
  POISON,//3
  GROUND,//4
  ROCK,//5
  BUG,//6
  GHOST,//7
  STEEL,//8
  FIRE,//9
  WATER,//10
  GRASS,//11
  ELECTRIC,//12
  PSYCHIC,//13
  ICE,//14
  DRAGON,//15
  DARK,//16
  FAIRY,//17
} Type;

typedef enum {
  NO_STATENUM = -1,
  ATT,//1
  DEF,//0
  SPD,//2
  ACC,//3
} StatEnum;

typedef enum {
  NO_AILMENT = -1,
  PARALYSIS,//0
  SLEEP,//2
  FREEZE,//3
  BURN,//4
  POISONING,//5
  CONFUSION,//6
} AilmentEnum;

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
} Pokemon;

typedef enum {
  IN_BATTLE,//0
  OUT_BATTLE,//1
  IN_N_OUT_BATTLE,//2
} ItemUse;

typedef struct {
  char *name;
  int name_length;
  int id;
  char *description;
  int description_length;
  int count;
  ItemUse usable_time;//0 if useable in of battle, 1 out battle, 2 both
} BagItem;

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
} Player;

#endif