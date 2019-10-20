# -*- coding: utf-8 -*-
"""
Created on Fri Oct  5 21:39:13 2018

@author: rgraptin
"""
import requests
import json
import random

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
TRANSLATION_FILE = "../pokedex.json"
POKEMONS_FILE = "../pokemons.txt"

# API link
REQUEST_URL = "https://pokeapi.co/api/v2/"
data = "pokemon/" # 808/
evo = "evolution-chain/" # 808/
j_start = 1

def getFrenchNameFromId(id) :
    with open (TRANSLATION_FILE, 'r') as f:
        jsonf = json.load(f)
        return jsonf[id-1]["name"]["french"]

def getFrenchNameFromName(name) :
    with open (TRANSLATION_FILE, 'r') as f:
        jsonf = json.load(f)
        id = 0
        while jsonf[id]["name"]["english"] != name :
            id = id + 1
        return jsonf[id]["name"]["french"]

def addEvolution(name, data) :
    frst_evo = data["chain"]
    if len(frst_evo["evolves_to"]) > 0 :
        scd_evo = frst_evo["evolves_to"][0]
        if frst_evo["species"]["name"] == name :
            evolution_name = scd_evo["species"]["name"]
            evo_lvl = scd_evo["evolution_details"][0]["min_level"]
            if evo_lvl is None :
                evo_lvl = random.randint(10, 20)
            evolution_name = getFrenchNameFromName(evolution_name.capitalize())
            return "|"+str(evo_lvl)+" "+str(len(evolution_name))+" "+evolution_name
        else :
            if len(scd_evo["evolves_to"]) > 0 :
                thrd_evo = scd_evo["evolves_to"][0]
                if scd_evo["species"]["name"] == name :
                    evolution_name = thrd_evo["species"]["name"]
                    evo_lvl = thrd_evo["evolution_details"][0]["min_level"]
                    if evo_lvl is None :
                        evo_lvl = scd_evo["evolution_details"][0]["min_level"]
                        if evo_lvl is None :
                            evo_lvl = random.randint(20, 35)
                        else :
                            evo_lvl += random.randint(10, 15)
                    evolution_name = getFrenchNameFromName(evolution_name.capitalize())
                    return "|"+str(evo_lvl)+" "+str(len(evolution_name))+" "+evolution_name
                else :
                    if thrd_evo["species"]["name"] == name :
                        return "|-1 0 "
                    else :
                        return ""
            else : 
                if scd_evo["species"]["name"] == name :
                    return "|-1 0 "
                else :
                    return ""
    else :
        if frst_evo["species"]["name"] == name :
            return "|-1 0 "
        else :
            return ""

def del_accent(line):
        """ delete accents """
        accents = { 'a': ['à', 'ã', 'á', 'â'],
                    'e': ['é', 'è', 'ê', 'ë'],
                    'i': ['î', 'ï'],
                    'u': ['ù', 'ü', 'û'],
                    'o': ['ô', 'ö'],
                    'c': ['ç'],
                    'A': ['À', 'Ã', 'Á', 'Â'],
                    'E': ['É', 'È', 'Ê', 'Ë'],
                    'I': ['Î', 'Ï'],
                    'U': ['Ù', 'Ü', 'Û'],
                    'O': ['Ô', 'Ö'],
                    ' F': ['♀'],
                    ' M': ['♂'],
                    '\'' : ['’'] }
        for (char, accented_chars) in accents.items():
            for accented_char in accented_chars:
                line = line.replace(accented_char, char)
        return line

def getLine (id, data) :
    """
    @array data : json data
    Generate into a line all the data
    @return The line with the Julien format
    """

    name = getFrenchNameFromId(id)
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

    global j_start
    j = j_start
    count = 0
    trouve = False
    while not trouve and count < NUMBER_OF_PERSO :
        # We generate the url for the evolution request
        url = REQUEST_URL + evo + str(j) + "/"
        # Get the value of the page
        r = requests.get(url)
        # Add evolution data to line
        evo_line = addEvolution(data["species"]["name"], r.json())
        if evo_line != "" :
            line += evo_line
            trouve = True
        else :
            count += 1
            j += 1
            j = j % NUMBER_OF_PERSO
            if j == 0 :
                j = 1
    
    if not trouve : 
        line += "|-1 0 "

    j_start = j
    line = del_accent(line)
    print(str(id) + ": "+ line)
    return line
    

# Open the file on write mode
with open (POKEMONS_FILE, 'w') as f:

    # Until we get the exception
    for i in range (1, NUMBER_OF_PERSO) :
        # We generate the url for the pokemon request
        url = REQUEST_URL + data + str(i) + "/"
        # Get the value of the page
        r = requests.get(url)
        # Get the ligne from the data convert to json
        line = getLine(i, r.json())
        
        # Write on our file the data
        f.write(line + "\n")
