# -*- coding: utf-8 -*-
"""
Created on Fri Oct  5 21:39:13 2018

@author: rgraptin
"""
import requests
import json


def type_to_int(type):
    """ convert a type string to an integer """
    res = -1
    if type == "normal":
        res = 0
    elif type == "fighting":
        res = 1
    elif type == "flying":
        res = 2
    elif type == "poison":
        res = 3
    elif type == "ground":
        res = 4
    elif type == "rock":
        res = 5
    elif type == "bug":
        res = 6
    elif type == "ghost":
        res = 7
    elif type == "steel":
        res = 8
    elif type == "fire":
        res = 9
    elif type == "water":
        res = 10
    elif type == "grass":
        res = 11
    elif type == "electric":
        res = 12
    elif type == "psychic":
        res = 13
    elif type == "ice":
        res = 14
    elif type == "dragon":
        res = 15
    elif type == "dark":
        res = 16
    elif type == "fairy":
        res = 17
    return res

NUMBER_OF_PERSO = 152
FR_LINK = "http://ray0.be/pokeapi/pokemon-row/fr/"


def getLine (id, data) :
    """
    @array data : json data
    Generate into a line all the data
    @return The line with the Julien format
    """

    url = FR_LINK + str(id) + "/"
    # Get the value of the page
    r = requests.get(url).json()


    name = r["data"]["nom_fr"]
    size_name = len(name)
    lvl = 1
    xp = 0


    # Get the type
    type_1 = type_to_int(data["types"][0]["type"]["name"])
    # Maybe only 1 type so need to catch the exception
    try :
        type_2 = type_to_int(data["types"][1]["type"]["name"])
    except :
        type_2 = -1

    # Stats
    stats = data["stats"]
    speed = stats[0]["base_stat"]
    defence = stats[3]["base_stat"]
    attack = stats[4]["base_stat"]
    hp = stats[5]["base_stat"]

    base_xp = data["base_experience"]

    line =  str(size_name) + " " +str(name) + "|" +  str(lvl) + " " + str(xp) + " " + str(type_1)
    line += " " + str(type_2) + "|" + str(base_xp) + "|" + str(hp) + " " + str(attack) + " " + str(defence) + " " + str(speed)

	# Add new items
	# line += "," + new_items

    print(str(id) + ": "+ line)
    return line




# API link
REQUEST_URL = "https://pokeapi.co/api/v2/"
data = "pokemon/" # 808/


# Open the file on write mode
with open ("../pokemons.txt", 'w') as f:

    # Until we get the exception
    for i in range (1, NUMBER_OF_PERSO) :
        # We generate the url for the request
        url = REQUEST_URL + data + str(i) + "/"
        # Get the value of the page
        r = requests.get(url)
        # Get the ligne from the data convert to json
        line = getLine(i, r.json())
        # Write on our file the data
        f.write(line + "\n")
