#ifndef TYPEDEF_H
#define TYPEDEF_H

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
  DEF,
  ATT,
  SPD,
  ACC,
} StatEnum;

typedef enum {
  NO_AILMENT = -1,
  PARALYSIS,
  SLEEP,
  FREEZE,
  BURN,
  POISONING,
  CONFUSION,
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

typedef struct {
  int xy;
  char pos;
  char char_at_pos;
  int pokeball_count;
  Pokemon pkmns[6];
  char pseudo[50];
  int pkmn_count;
  int pseudo_length;
  int money;
} Player;

#endif