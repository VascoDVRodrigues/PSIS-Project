#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BOLDWHITE "\033[1m\033[37m" /* Bold White */

#include "KVS-lib.h"

// exemple of function to be executed whenever a key's value is updated
void keyALterada(char *key) {
	printf("\n\t %s was altered. This is the callback function.\n", key);

	// put_value(key, "some new key");

	return;
}

int main() {
	char group_id[1024];
	char secret[1024];

	char *result;
	int a;

	while (1) {
		printf(BOLDWHITE "Enter GroupId: ");
		scanf("%s", group_id);
		printf("Enter Secret: ");
		scanf("%s", secret);

		// establishes connection with the local server
		a = establish_connection(group_id, secret);
		if (a == 0) {
			printf("Established conNEECtion ✔️\n");
			break;
		} else if (a == -1) {
			printf("Correct Group, wrong password ❌\n");
			;
		} else if (a == -2) {
			printf("No Group with same GroupId ❌\n");
		} else if (a == -3) {
			printf("Error in connection to server ❌\n");
		} else if (a == -5) {
			printf("Full server, try again later 👥\n");
		} else if (a == -4) {
			printf("Timed out ⌛️\n");
		}
	}

	a = put_value(
		"big put",
		" {Man} Once upon a time there was a lovely princess.\nBut she had an enchantment upon her of a fearful sort which could only \nbe "
		"broken by love's first kiss.\nShe was locked away in a castle guarded by a terrible fire-breathing \ndragon.\nMany brave knigts "
		"had attempted to free her from this dreadful prison, \nbut non prevailed.\nShe waited in the dragon's keep in the highest room of "
		"the tallest \ntower for her true love and true love's first kiss.\n{Laughing} \nLike that's ever gonna happen.\n{Paper Rusting, "
		"Toilet Flushes}\nWhat a load of - \n\nSomebody once told me the world is gonna roll me\nI ain't the sharpest tool in the "
		"shed\nShe was lookin' kind of dumb with her finger and her thumb\nIn the shape of an "
		L" on her forehead\nThe years start comin' and they don't stop comin'\nFed to the rules and hit the ground runnin'\nDidn't make "
		L"sense not to live for fun\nYour brain gets smart but your head gets dumb\nSo much to do so much to see\nSo what's wrong with "
		L"takin' the backstreets\nYou'll never know if you don't go\nYou'll never shine if you don't glow\nHey, now You're an "
		L"all-star\nGet your game on, go play\nHey, now You're a rock star\nGet the show on, get paid\nAnd all that glitters is gold\nOnly "
		L"shootin' stars break the mold\nIt's a cool place and they say it gets colder\nYou're bundled up now but wait till you get "
		L"older\nBut the meteor men beg to differ\nJudging by the hole in the satellite picture\nThe ice we skate is gettin' pretty "
		L"thin\nThe water's getting warm so you might as well swim\nMy world's on fire\nHow 'bout yours\nThat's the way I like it and I'll "
		L"never get bored\nHey, now, you're an all-star\n{Shouting}\nGet your game on, go play\nHey, now You're a rock star\nGet the show "
		L"on, get paid\nAnd all that glitters is gold\nOnly shootin' stars break the mold\n{Belches}\nGo!\nGo!\n{Record Scratching}\nGo. "
		L"Go.Go.\nHey, now, you're an all-star\nGet your game on, go play\nHey, now You're a rock star\nGet the show on, get paid\nAnd all "
		L"that glitters is gold\nOnly shootin' stars break the mold\n\n-Think it's in there?\n-All right. Let's get it!\n-Whoa. Hold on. "
		L"Do you know what that thing can do to you?\n-Yeah, it'll grind your bones for it's bread.\n{Laughs}\n-Yes, well, actually, that "
		L"would be a gaint.\nNow, ogres - - They're much worse.\nThey'll make a suit from your freshly peeled skin.\n-No!\n-They'll shave "
		L"your liver. Squeeze the jelly from your eyes!\nActually, it's quite good on toast.\n-Back! Back, beast! Back! I warn "
		L"ya!\n{Gasping}\n-Right.\n{Roaring}\n{Shouting}\n{Roaring}\n{Whispers} This is the part where you run "
		L"away.\n{Gasping}\n{Laughs}\n{Laughing} And stay out!\nWanted. Fairy tale creatures.\n{Sighs}\n{Man's voice} All right. This "
		L"one's full.\n-Take it away!\n{Gasps}\n-Move it along. Come on! Get up!\n-Next!\n-Give me that! Your fiying days are "
		L"over.\nThat's 20 pieces of silver for the witch. Next!\n-Get up! Come on!\n-Twenty pieces.\n{Thudding}\n-Sit down there!\n-Keep "
		L"quiet!\n{Crying}\n-This cage is too small.\n-Please, don't turn me in. I'll never be stubborn again.\nI can change. Please! Give "
		L"me another chance!\n-Oh, shut up.\n-Oh!\n-Next!\n-What have you got?\n-This little wooden puppet.\n-I'm not a puppet. I'm a real "
		L"boy.\n-Five shillings for the possessed toy. Take it away.\n-Father, please! Don't let them do this!\n-Help me!\n-Next! What "
		L"have you got?\n-Well, I've got a talking donkey.\n{Grunts}\n-Right. Well, that's good for ten shillings, if you can prove "
		L"it.\n-Oh, go ahead, little fella.\n-Well?\n-Oh, oh, he's just - - He's just a little nervous.\nHe's really quite a chatterbox. "
		L"Talk, you boneheaded dolt - -\n-That's it. I've heard enough. Guards!\n-No, no, he talks! He does. I can talk. I love to "
		L"talk.\nI'm the talkingest damn thing you ever saw.\n-Get her out of my sight.\n-No, no! I swear! Oh! He can "
		L"talk!\n{Gasps}\n-Hey! I can fly!\n-He can fly!\n-He can fly!\n-He can talk!\n-Ha, ha! That's right, fool! Now I'm a flying, "
		L"talking donkey.\nYou might have seen a housefly, maybe even a superfly\nbut I bet you ain't never seen a donkey fly. Ha, "
		L"ha!\nOh-oh.\n{Grunts}\n-Seize him!\n-After him! He's getting away!\n{Grunts, Gasps}\n{Man}\n-Get him! This way! Turn!\n-You "
		L"there. Orge!\n-Aye?\n-By the order of Lord Farquaad I am authorized to place you both under \narrest\nand transport you to a "
		L"designated..... resettlement facility.");
	if (a == 0) {
		printf("Value stored ✔️\n");
	} else if (a == -2) {
		printf("Grupo foi apagado ❌\n");
		return -1;
	} else {
		printf("Error ❌\n");
		return -1;
	}

	a = get_value("big put", result);

	if (a == 0) {
		printf("Value retrieved ✔️\n");
		printf("Value: %s\n", result);
	} else if (a == -2) {
		printf("Grupo foi apagado ❌\n");
		return -1;
	} else {
		printf("Error ❌\n");
		return -1;
	}
	return 0;
}