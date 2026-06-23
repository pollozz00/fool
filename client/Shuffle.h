#pragma once
#include "Card.h"
#include "vector"

class Shuffle
{
private:
    std::vector<Card> temp;

public:

    void FisherYates(std::vector<Card>& deck, int size)
    {
        for (int i = size - 1; i > 0; --i)
        {
            int j = rand() % (i + 1);

            Card tmp = deck[i];
            deck[i] = deck[j];
            deck[j] = tmp;
        }
    }


    void Riffle(std::vector<Card>& deck, int size)
    {
        temp.resize(size);
        int mid = size / 2;
        int i = 0, l = 0, r = mid;

        while (l < mid || r < size)
        {
            if (l < mid && (r >= size || rand() % 2))
                temp[i++] = deck[l++];
            if (r < size && rand() % 2)
                temp[i++] = deck[r++];
        }

        for (int k = 0; k < size; ++k)
            deck[k] = temp[k];
    }

    void Overhand(std::vector<Card>& deck, int size)
    {
        temp.resize(size);
        int pos = size;

        while (pos > 0)
        {
            int chunk = rand() % 5 + 1;
            if (chunk > pos) chunk = pos;

            for (int i = 0; i < chunk; ++i)
                temp[size - pos + i] = deck[pos - chunk + i];

            pos -= chunk;
        }

        for (int i = 0; i < size; ++i)
            deck[i] = temp[i];
    }


};

