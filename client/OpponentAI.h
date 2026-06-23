#pragma once
#include "Card.h"
#include <vector>

class OpponentAI
{
private:
	int difficulty = 1;


public:
	int GetDifficulty() {
		return difficulty;
	}

	void SetDifficulty(int d) {
		difficulty = d;
	}

	int ChooseAttackCard(std::vector <Card>& hand, int count, Suit trump)
	{
		if (count == 0) return -1;

		//EASY
		if (GetDifficulty() == 1) {
			int index = -1;
			Rank minCard = (Rank)100;

			for (int i = 0; i < count; i++) {
				if (hand[i].rank < minCard) {
					minCard = hand[i].rank;
					index = i;
				}
			}
			return index;
		}

		//MEDIUM
		else if (GetDifficulty() == 2) {
			int index = -1;
			Rank minCard = (Rank)100;

			for (int i = 0; i < count; i++) {
				if (hand[i].suit != trump && hand[i].rank < minCard) {
					minCard = hand[i].rank;
					index = i;
				}
			}

			if (index == -1) {
				for (int i = 0; i < count; i++) {
					if (hand[i].rank < minCard) {
						minCard = hand[i].rank;
						index = i;
					}
				}
			}
			return index;

		}

		//HARD
		else if (GetDifficulty() == 3) {
			int index = -1;
			Rank bestRank = (Rank)100;

			int trumpCount = 0;
			for (int i = 0; i < count; i++) {
				if (hand[i].suit == trump)
					trumpCount++;
			}

			for (int i = 0; i < count; i++) {
				if (hand[i].suit != trump && hand[i].rank < bestRank) {
					bestRank = hand[i].rank;
					index = i;
				}
			}

			if (index == -1) {
				for (int i = 0; i < count; i++) {
					if (hand[i].rank < bestRank) {
						bestRank = hand[i].rank;
						index = i;
					}
				}
				return index;
			}

			if (trumpCount <= 1) {
				Rank minTrump = Ace;
				int trumpIndex = -1;

				for (int i = 0; i < count; i++) {
					if (hand[i].suit == trump && hand[i].rank < minTrump) {
						minTrump = hand[i].rank;
						trumpIndex = i;
					}
				}

				if (trumpIndex != -1 && minTrump <= bestRank)
					return trumpIndex;
			}

			return index;
		}

	}
	int ChooseDefenseCard(std::vector <Card>& hand, int count, Card attackingCard, Suit trump) 
	{
		if (count == 0)	return -1;
		
		int index = -1;
		Rank minCard = (Rank)100;
		Rank minTrump = (Rank)100;

		//EASY
		if (GetDifficulty() == 1) {
			for (int i = 0; i < count; i++) {
				if (hand[i].suit == attackingCard.suit && hand[i].rank > attackingCard.rank) {
					index = i;
					return index;
				}

				if (hand[i].suit == trump && attackingCard.suit != trump) {
					index = i;
					return index;
				}
			}
		}

		//MEDIUM
		else if (GetDifficulty() == 2) {
			for (int i = 0; i < count; i++) {
				if (hand[i].suit == attackingCard.suit &&
					hand[i].rank > attackingCard.rank &&
					hand[i].rank < minCard) {
					minCard = hand[i].rank;
					index = i;
				}
			}


			if (index == -1 && attackingCard.suit != trump) {
				for (int i = 0; i < count; i++) {
					if (hand[i].suit == trump && hand[i].rank < minTrump) {
						minTrump = hand[i].rank;
						index = i;
					}
				}
			}
		}
		//HARD
		else if (GetDifficulty() == 3) {
			int bestIndex = -1;
			Rank bestRank = (Rank)100;

			for (int i = 0; i < count; i++) {
				if (hand[i].suit == attackingCard.suit &&
					hand[i].rank > attackingCard.rank) {
					if (bestIndex == -1 || hand[i].rank < bestRank) {
						bestRank = hand[i].rank;
						bestIndex = i;
					}
				}
			}

			if (bestIndex == -1 && attackingCard.suit != trump) {
				bestRank = (Rank)100;
				for (int i = 0; i < count; i++) {
					if (hand[i].suit == trump && hand[i].rank < bestRank) {
						bestRank = hand[i].rank;
						bestIndex = i;
					}
				}
			}

			if (bestIndex == -1 && attackingCard.suit == trump) {
				bestRank = (Rank)100;
				for (int i = 0; i < count; i++) {
					if (hand[i].suit == trump && hand[i].rank > attackingCard.rank) {
						if (hand[i].rank < bestRank) {
							bestRank = hand[i].rank;
							bestIndex = i;
						}
					}
				}
			}

			return bestIndex;
		}

		return index;
	}

};

