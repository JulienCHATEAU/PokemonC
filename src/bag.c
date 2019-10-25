#include "bag.h"
#include "fileManager.h"
#include "pokemon.h"
#include "typedef.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Fills a bag item using the file describing all the existing items and its id
 * id : the bag item's id
 * bag_item : the adress of a bag item structure to fill
 */
void fillBagItem(int id, BagItem *bag_item) {
  FILE *bag_items = openFile(BAG_ITEM_PATH, "r");
  skipLines(bag_items, id);
  fscanf(bag_items, "%d|", &bag_item->name_length);
  bag_item->name = malloc(sizeof(char) * bag_item->name_length + 1);
  fscanf(bag_items, "%[^|]|", bag_item->name);
  fscanf(bag_items, "%d|", &bag_item->description_length);
  bag_item->description =
      malloc(sizeof(char) * bag_item->description_length + 1);
  fscanf(bag_items, "%[^|]|", bag_item->description);
  int tmp_usable_time = 0;
  fscanf(bag_items, "%d", &tmp_usable_time);
  bag_item->usable_time = (ItemUse)tmp_usable_time;
  bag_item->count = 0;
  bag_item->id = id;
  closeFile(bag_items);
}

/* Creates a bag item depending on its id
 * id : the bag item's id
 * return the corresponding bag item
 */
BagItem createBagItem(int id) {
  BagItem bag_item;
  fillBagItem(id, &bag_item);
  return bag_item;
}

/* Inits the player bag
 * player : the player
 */
void initBag(Player *player) {
  BagItem bi;
  for (int i = 0; i < BAG_SIZE; i++) {
    player->bag[i] = bi;
    player->bag[i].id = -1; // empty bag item
    player->bag[i].count = 0;
  }
}

/* Inits the player picked items
 * player : the player
 */
void initPikedItem(Player *player) {
  for (int i = 0; i < ITEM_COUNT; i++) {
    player->item_picked[i].x_map = 0;
    player->item_picked[i].y_map = 0;
    player->item_picked[i].xy = 0;
  }
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

bool possessAllKeys(Player *player) {
  int count = 0;
  int i = 0;
  while (count != 5 && i < player->bag_item_count) {
    if (player->bag[i].id >= 5 && player->bag[i].id <= 9) {
      count++;
    }
    i++;
  }
  return count == 5;
}

int itemCount(Player *player, int id) {
  int index = possessBagItem(player, id);
  if (index != -1) {
    index = player->bag[index].count;
  }
  return index;
}

/* Checks wheter the item is usable in the current game state
 * game_state : the current game state
 * usable_time : time when a bag item is useable : 0 in battle, 1 out battle, 2
 * both, 3 nowhere return true if the item is usable, false otherwise
 */
bool isItemUsable(int game_state, int usable_time) {
  return (usable_time == 2 || game_state == usable_time) && usable_time != 3;
}

/* Adds a specified count of a bag item to the player bag
 * player : the player
 * id : the bag item's id
 * count : the item count to add
 */
BagItem addBagItemPlayer(Player *player, int id, int count) {
  if (count > 0) {
    int index = possessBagItem(player, id);
    if (index == -1) {
      player->bag[player->bag_item_count] = createBagItem(id);
      player->bag[player->bag_item_count].count += count;
      player->bag_item_count++;
      return player->bag[player->bag_item_count - 1];
    } else {
      player->bag[index].count += count;
      return player->bag[index];
    }
  }
  return player->bag[0];
}

void removeItem(Player *player, int item_index, int count) {
  player->bag[item_index].count -= count;
  if (player->bag[item_index].count <= 0) {
    for (int i = item_index; i < player->bag_item_count - 1; i++) {
      player->bag[i] = player->bag[i + 1];
    }
    player->bag_item_count--;
    player->bag[player->bag_item_count].count = 0;
    player->bag[player->bag_item_count].id = -1;
  }
}

/* Uses a potion on the targetted pokemon
 * player : the player
 * targetted_pkmn : the index of the pokemon to heal
 */
int usePotion(Player *player, int targetted_pkmn) {
  Pokemon *pkmn_to_heal = &(player->pkmns[targetted_pkmn]);
  int heal_amount = -1;
  if (pkmn_to_heal->stats.hp > 0) {
    removeItem(player, possessBagItem(player, POTION_ID), 1);
    heal_amount = pkmn_to_heal->stats.hp_max * POTION_HEAL / 100;
    heal(pkmn_to_heal, heal_amount);
  }
  return heal_amount;
}

/* Uses a total soin on the targetted pokemon
 * player : the player
 * targetted_pkmn : the index of the pokemon to heal
 */
void useTotalSoin(Player *player, int targetted_pkmn) {
  Pokemon *pkmn_to_heal = &(player->pkmns[targetted_pkmn]);
  if (pkmn_to_heal->crt_ailments[0] != NO_AILMENT) {
    removeItem(player, possessBagItem(player, TOTAL_SOIN_ID), 1);
    removeAllAilments(pkmn_to_heal);
  }
}