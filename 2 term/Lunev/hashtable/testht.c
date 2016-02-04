#include "hashtable.h"

int UNLUCK = 10;

int main()
{
	int my_max_hashvalue = 100;
	int my_max_key_length = 20;
	int my_max_blob_length = 20;
	int error_value = 0;

	UNLUCK = 0;

	hashtable_t	my_hashtable1 = hashtable_construct (my_max_hashvalue, &error_value);

	hashtable_clear (my_hashtable1);
	hashtable_add (my_hashtable1, "hello", " world!", 5);
	hashtable_add (my_hashtable1, "hell", "is world!", 6);
	hashtable_add (my_hashtable1, "help", "i need smb", 7);

	hashtable_del (my_hashtable1, "hell");
	hashtable_del (my_hashtable1, "help");
	hashtable_del (my_hashtable1, "hel");
	hashtable_del (my_hashtable1, "hello");
	hashtable_destruct (my_hashtable1);

	UNLUCK = 100;

	hashtable_t my_hashtable = hashtable_construct (my_max_hashvalue, NULL);
	my_hashtable = hashtable_construct (999999, &error_value);
	
	UNLUCK = 10;

	register int i = 0;

	for (i = 0; i < 10; i++)
	{
		my_hashtable = hashtable_construct (my_max_hashvalue, &error_value);

		hashtable_clear (my_hashtable);
		hashtable_add (my_hashtable, "hello", " world!", 5);
		hashtable_add (my_hashtable, "hell", "is world!", 6);
		hashtable_add (my_hashtable, "help", "i need smb", 7);

		hashtable_del (my_hashtable, "hell");
		hashtable_del (my_hashtable, "help");
		hashtable_del (my_hashtable, "hel");
		hashtable_del (my_hashtable, "hello");
		hashtable_destruct (my_hashtable);
	}

	UNLUCK = 100;

	my_hashtable = hashtable_construct (my_max_hashvalue, &error_value);

	UNLUCK = 0;

	my_hashtable = hashtable_construct (my_max_hashvalue, &error_value);

	hashtable_clear (my_hashtable);

	hashtable_add (my_hashtable, "hello", ", world!", 5);

	hashtable_add (my_hashtable, "hi", ", username!", 10);

	hashtable_add (my_hashtable, NULL, ", username!", 10);
	hashtable_add (my_hashtable, "hello!, ", NULL, 10);
	hashtable_add (my_hashtable, "hijdiodnoqasnduianiasniucasnf icusabnisanciuasnsauidniasudhasuidnasidsabdiasndnasiodno", NULL, 10);
	hashtable_add (my_hashtable, "hi_there", ", username!", -1);
	hashtable_add (my_hashtable, "hi", ", username!", 100000);


	hashtable_del (my_hashtable, NULL);
	hashtable_del (my_hashtable, "hijdiodnoqasnduianiasniucasnf icusabnisanciuasnsauidniasudhasuidnasidsabdiasndnasiodno");

	UNLUCK = 1;
	hashtable_del (my_hashtable, "hello");
	UNLUCK = 0;

	hashtable_del (my_hashtable, "hello");
	hashtable_clear (NULL);

	hashtable_del (my_hashtable, "hello");

	ht_iterator_t it0 = iterator_construct (NULL, &error_value);
	it0 = iterator_construct (my_hashtable, NULL);
	UNLUCK = 100;
	it0 = iterator_construct (my_hashtable, &error_value);
	UNLUCK = 0;

	char key [my_max_key_length];
	char blob[my_max_blob_length];
	int blob_size;

	iterator_get (NULL, key, blob, &blob_size);
	iterator_get (it0, key, blob, NULL);

	iterator_get (it0, key, NULL, &blob_size);
	iterator_get (it0, NULL, blob, &blob_size);

	hashtable_add (my_hashtable, "hello", " world!", 5);
	hashtable_add (my_hashtable, "hell", "is world!", 6);
	hashtable_add (my_hashtable, "help", "i need smb", 7);

	hashtable_destruct (my_hashtable);

	iterator_destruct (it0);

	my_hashtable = hashtable_construct (my_max_hashvalue, &error_value);

	ht_iterator_t it2 = iterator_construct (my_hashtable, &error_value);

	iterator_destruct (it2);

	ht_iterator_t it1 = iterator_construct (my_hashtable, &error_value);

	hashtable_clear (my_hashtable);
	hashtable_add (my_hashtable, "hello", "worlddd!", 7);
	hashtable_add (my_hashtable, "hell", "is world!", 6);
	hashtable_add (my_hashtable, "help", "i need smb", 10);
	hashtable_add (my_hashtable, "not", "just anbd!", 10);

		memset (blob, 0, my_max_blob_length);
	while (iterator_get (it1, key, blob, &blob_size) >= 0)
	{
		fprintf(stderr, "%s\n", blob);
		memset (blob, 0, my_max_blob_length);
	}

	hashtable_destruct (my_hashtable);

	return 0;
}