
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dice.h"

// TODO: define the struct called Armor with the following fields
// - id: character array of 20
// - armor_class: an integer
struct Armor{
    char id[20];
    int armor_class;
};
typedef struct Armor Armor;

// TODO: define the struct called AttackType with the following fields
// - id: character array of 20
// - dice_count: should be an integer
// - dice_type: use the DiceType enum from dice.h
// - damage_bonus: an other integer
struct AttackType{
    char id[20];
    int dice_count;
    DiceType dice_type;
    int damage_bonus;
};
typedef struct AttackType AttackType;

// TODO: a struct called Creature with the following fields
// - id: character array of 20
// - hp: an integer holding the health points of the creature
// - attack_bonus: an other integer
// - armor: a field with type Armor
// - attack_type: a field with type AttackType
struct Creature{
    char id[20];
    int hp;
    int attack_bonus;
    Armor armor;
    AttackType attack_type;
};
typedef struct Creature Creature;

// TODO: implement the following function that takes a file handle and parses
// input in the format <id> <armor_class>. It should use the fscanf function for
// parsing and return an Armor. For example:
//
//   "leather 11"
//
// means a leather armor (id) with armor class 11. The id of the armor doesn't
// have whitespace characters in it.
struct Armor parse_armor(FILE *fp){
    Armor current_armor;

    fscanf(fp, "%s %d", current_armor.id, &current_armor.armor_class);

    return current_armor;
}

// TODO: implement the following function that takes a file handle and parses
// the input in <id> <dice_count> <dice_type> <bonus> format. It should use the
// fscanf function for parsing and return an AttackType. For example:
//
//   "dagger 1 D4 2"
//
// means a dagger (id) with damage defined as 1d4+2. The id of the attack type
// doesn't have whitespace characters in it.
struct AttackType parse_attack_type(FILE *fp){
    AttackType current_attack_type;
    const char *diceNames[] = {"D4", "D6", "D8", "D10", "D12", "D20", "D100"};
    DiceType diceValues[] = {D4, D6, D8, D10, D12, D20, D100};

    char tmp_dice_type[5];

    fscanf(fp, "%s %d %4s %d", current_attack_type.id, &current_attack_type.dice_count, tmp_dice_type, &current_attack_type.damage_bonus);

    int index = 0;
    while (index<7 && strcmp(tmp_dice_type, diceNames[index])) index++;
    current_attack_type.dice_type = diceValues[index];

    return current_attack_type;
}

// TODO: implement the following function that takes a file handle and parses
// input in the format of <id> <hp> <attack_bonus> <armor_id> <attack_type_id>.
//
// The function should use fscanf to parse the line and return a Creature.
// For example the line
//
//   "Hero   30 6 leather    dagger"
//
// translates to a creature with id 'Hero' having 30 hp, and an attack bonus
// of 6. He is equipped with leather armor and a dagger. You can suppose that an
// armor with the id 'leather' is provided in the armors array, as well as an
// attack type with the id 'dagger'. You need to look them up by their ids; you
// can use the `strcmp` function for that. The id of the creature doesn't
// contain whitespace characters.
struct Creature parse_creature(FILE *fp, struct Armor armors[], int armor_count,struct AttackType attack_types[], int attack_type_count){
    Creature current_creature;

    fscanf(fp, "%s %d %d %s %s", current_creature.id, &current_creature.hp, &current_creature.attack_bonus, current_creature.armor.id, current_creature.attack_type.id);

    int index = 0;
    while (index < armor_count && strcmp(current_creature.armor.id, armors[index].id)) index++;
    current_creature.armor.armor_class = armors[index].armor_class;

    index = 0;
    while (index < attack_type_count && strcmp(current_creature.attack_type.id, attack_types[index].id)) index++;
    current_creature.attack_type.dice_type = attack_types[index].dice_type;
    current_creature.attack_type.dice_count = attack_types[index].dice_count;
    current_creature.attack_type.damage_bonus = attack_types[index].damage_bonus;

    return current_creature;
}

// TODO: using the `roll` function from dice.h implement the fight between
// two creatures as follows.
// - The fight is turn-based. c1 attacks first, then c2, and they
//   alternate until one of them is reduced to 0 hit points or below.
// - On its turn, the attacker makes an attack roll using a single D20 dice and
//   its own attack bonus. (Use the provided roll function).
// - The attack hits if the attack roll is greater than or equal to the
//   defender's armor class.
// - On a hit, damage is computed from the attacker's AttackType using the
//   dice rolling function again, but this time you 'roll' with the values
//   stored in the AttackType (dice type, count and damage_bonus).
//   The defender's hp is then reduced by this damage value.
// - If, after taking damage, a creature's hp reaches 0 the fight ends
// immediately
//   and the attacking creature is considered the winner.
// - The function returns the pointer to the winner.
struct Creature *fight(struct Creature *c1, struct Creature *c2){
    int attack = roll(D20, 1, c1->attack_bonus);
    if (attack >= c2->armor.armor_class){
        int damage = roll(c1->attack_type.dice_type, c1->attack_type.dice_count, c1->attack_type.damage_bonus);
        c2->hp -= damage;
    }

    if (c2->hp <= 0)
        return c1;
    return fight(c2, c1);
};

// TODO: main should take a filename argument, open the file, parse
// its contents and simulate the fights. Print out the first character
// of the winner of each fight.
int main(int argc, char *argv[]){
    FILE* file = fopen(argv[1], "r");

    int armor_amount;
    fscanf(file, "%d", &armor_amount);
    Armor *armors = (Armor*)malloc(armor_amount*sizeof(Armor));
    if (armors == NULL){
        printf("Could not allocate memory for \"armors\"");
        return 1;
    }

    for (int i=0; i<armor_amount; i++){
        armors[i] = parse_armor(file);
    }

    int attack_type_amount;
    fscanf(file, "%d", &attack_type_amount);
    AttackType *attack_types = (AttackType*)malloc(attack_type_amount*sizeof(AttackType));
    if (attack_types == NULL){
        printf("Could not allocate memory for \"attack_types\"");
        return 1;
    }

    for (int i=0; i<attack_type_amount; i++){
        attack_types[i] = parse_attack_type(file);
    }

    int creature_amount;
    fscanf(file, "%d\n", &creature_amount);
    Creature *creatures = (Creature*)malloc(creature_amount*sizeof(Creature));
    if (creatures == NULL){
        printf("Could not allocate memory for \"creatures\"");
        return 1;
    }

    for (int i=0; i<creature_amount; i++){
        creatures[i] = parse_creature(file, armors, armor_amount, attack_types, attack_type_amount);
    }

    for (int i=0, j=1; j<creature_amount; i+=2, j+=2){
        Creature winner = *fight(&creatures[i], &creatures[j]);
        printf("%c", winner.id[0]);
    }

    free(armors);
    free(attack_types);
    free(creatures);

    fclose(file);
};
