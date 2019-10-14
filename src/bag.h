#include <stdbool.h>
#include "typedef.h"

#ifndef BAG_H
#define BAG_H

#define BAG_ITEM_PATH "data/bag_items.txt"
#define POKEBALL_ID 0
#define POTION_ID 1

#define POTION_HEAL 30

int possessBagItem(Player *player, int item);
BagItem createBagItem(int id);
void fillBagItem(int id, BagItem *bag_item);
void addBagItemPlayer(Player *player, int id, int count);
void initBag(Player *player);
void removeItem(Player *player, int item_index, int count);
bool possessAllKeys(Player *player);
int itemCount(Player *player, int id);
int usePotion(Player *player, int targetted_pkmn);

#endif
