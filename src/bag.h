#include "typedef.h"

#ifndef BAG_H
#define BAG_H

#define BAG_ITEM_PATH "data/bag_items.txt"

int possessBagItem(Player *player, int item);
BagItem createBagItem(int id);
void fillBagItem(int id, BagItem *bag_item);
void addBagItemPlayer(Player *player, int id, int count);
#endif