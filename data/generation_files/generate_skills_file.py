# -*- coding: utf-8 -*-
"""
Created on Tue Oct 16 16:02:35 2018

@author: jchateau
"""
import requests
import json

# NUMBER_OF_SKILLS = 20
NUMBER_OF_SKILLS = 220

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
                    '\'' : ['’'] }
        for (char, accented_chars) in accents.items():
            for accented_char in accented_chars:
                line = line.replace(accented_char, char)
        return line


def none_to_i(string, i):
    if string == None:
        return i
    else:
        return string

def stat_change_to_int(string):
    res = -1
    if string == "attack" or string == "special-attack":
        res = 0
    elif string == "defense" or string == "special-defense":
        res = 1
    elif string == "speed":
        res = 2
    elif string == "accuracy":
        res = 3
    return res

def ailment_to_int(string):
    res = -1
    if string == "paralysis":
        res = 0
    elif string == "sleep":
        res = 1
    elif string == "freeze":
        res = 2
    elif string == "burn":
        res = 3
    elif string == "poison":
        res = 4
    elif string == "confusion":
        res = 5
    elif string == "none":
        res = -1
    return res

def contains(string, tab):
    for i in range(0, len(tab)-1):
        if string == tab[i]:
            return True
    return False

def getLine (id, data, names_accepted) :
    """
    @array data : json data
    Generate into a line all the data
    @return The line with the Julien format
    """

    line = ""

    names = data["names"]

    i = 0
    while names[i]["language"]["name"] != "fr":
        i = i + 1
    name = del_accent(names[i]["name"]).replace("\n", "")
    contain = contains(name,names_accepted)
    if contain:
        size_name = len(name)

        # Carac
        power = none_to_i(data["power"], 0)

        accuracy = none_to_i(data["accuracy"], 100)
        if name == "Corps Perdu" or name == "Aurasphere" or name == "Feinte" or name == "Meteore":
            accuracy = 100

        pp = data["pp"]
        type = type_to_int(data["type"]["name"])
        priority = data["priority"]

        stat_changes = data["stat_changes"]
        stat_change = -1
        change = 0
        if len(stat_changes) :
            stat_change = stat_change_to_int(stat_changes[0]["stat"]["name"])
            change = stat_changes[0]["change"]

        stat_chance = 100
        if data["meta"]["stat_chance"] != 0:
            stat_chance = data["meta"]["stat_chance"]

        i = 0
        while data["flavor_text_entries"][i]["language"]["name"] != "fr":
            i = i + 1
        fr_flavor_text = del_accent(data["flavor_text_entries"][i]["flavor_text"])
        fr_flavor_text_length = len(fr_flavor_text)

        ailment = ailment_to_int(data["meta"]["ailment"]["name"])
        ailment_chance = data["meta"]["ailment_chance"]
        if ailment_chance == 0 and ailment != -1:
            ailment_chance = 100

        stat_change_target = 0
        if data["meta"]["category"]["name"] == "damage+raise":
            stat_change_target = 1
        if data["meta"]["category"]["name"] == "net-good-stats" and change > 0:
            stat_change_target = 1

        # Create a new variable to add to line
        # new_variable = data[...]


        line +=  str(size_name) + " " + str(name)
        line += "|" + str(power) + " " + str(accuracy) + " " + str(pp) + " " + str(type) + " " + str(priority)
        line += "|" + str(stat_change) + " " + str(change) + " " + str(stat_chance)
        line += "|" + str(ailment) + " " + str(ailment_chance)
        line += "|" + str(stat_change_target)
        line += "|" + str(fr_flavor_text_length) + " " + str(fr_flavor_text.replace("\n", " "))

        # Add new items in line variable
        # line += " " + str(new_variable)

        print(str(id) + " : " + line)

    return line




# API link
REQUEST_URL = "https://pokeapi.co/api/v2/"
data = "move/"


skills_name_tab = []
with open ("../skills_to_keep.txt", 'r') as skills_name_file:
    skills_name_tab = [del_accent(line.rstrip('\n')) for line in skills_name_file];
for i in range(0, len(skills_name_tab)):
    splitted_skill_name = skills_name_tab[i].split(" ");
    tmp = splitted_skill_name[1]
    for j in range(2, len(splitted_skill_name)):
        tmp += " " + splitted_skill_name[j]
    skills_name_tab[i] = tmp

skills_name_file.close()

# Open the file on write mode
with open ("../skills.txt", 'w') as f:

    # Until we get the exception
    i = 1
    cpt = 0
    while cpt != NUMBER_OF_SKILLS:
        # We generate the url for the request
        url = REQUEST_URL + data + str(i) + "/"
        # Get the value of the page
        r = requests.get(url)
        # Get the ligne from the data convert to json
        line = getLine(i, r.json(), skills_name_tab)
        if line != "":
            cpt = cpt + 1
            # Write on our file the data
            f.write(line + "\n")
        i = i + 1
f.close()
