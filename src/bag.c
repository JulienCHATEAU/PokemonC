#include <stdio.h>
#include <stdlib.h>
#include "bag.h"
#include "typedef.h"
#include "fileManager.h"

/* Fills a bag item using the file describing all the existing items and its id
* id : the bag item's id
* bag_item : the adress of a bag item structure to fill
*/
void fillBagItem(int id, BagItem *bag_item) {
  FILE *bag_items = openFile(BAG_ITEM_PATH, "r");
  skipLines(bag_items, id);
  fscanf(bag_items, "%d|", &bag_item->name_length);
  bag_item->name = malloc(sizeof(char)*bag_item->name_length+1);
  fscanf(bag_items, "%[^|]|", bag_item->name);
  fscanf(bag_items, "%d|", &bag_item->description_length);
  bag_item->description = malloc(sizeof(char)*bag_item->description_length+1);
  fscanf(bag_items, "%[^|]", bag_item->description);
  bag_item->count = 1;
  closeFile(bag_items);
}

/* Creates a bag item depending on its id
* id : the bag item's id
*       0 -> pokeball
*       1 -> potion
* return the corresponding bag item
*/
BagItem createBagItem(int id) {
  BagItem bag_item;
  fillBagItem(id, &bag_item);
  return bag_item;
}

/* Checks if the player possesses a specified item in his bag
* player : the player
* id : the item's id
* return 1 if the item is in the player bag, 2 otherwise
*/
int possessBagItem(Player *player, int id) {
  int i = 0;
  int index = -1;
  while (index == -1 && i < player->bag_item_count) {
    if (player->bag[i].id == id) {
      index = i;
    }
    i++;
  }
  return index;
}

/* Adds a specified count of a bag item to the player bag
* player : the player
* id : the bag item's id
* count : the item count to add
*/
void addBagItemPlayer(Player *player, int id, int count) {
  if (count > 0) {
    int index = possessBagItem(player, id);
    if (index == -1) {
      BagItem bag_item = createBagItem(id);
      player->bag[id] = bag_item;
      player->bag[id].count = count;
      player->bag_item_count++;
    } else {
      player->bag[id].count += count;
    }
  }
}