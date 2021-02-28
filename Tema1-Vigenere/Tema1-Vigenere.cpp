#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include<time.h>
#include <Windows.h>
#define IC_EN 0.065
using namespace std;
ifstream fin;
inline long long PerformanceCounter() noexcept
{
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);
	return li.QuadPart;
}
inline long long PerformanceFrequency() noexcept
{
	LARGE_INTEGER li;
	::QueryPerformanceFrequency(&li);
	return li.QuadPart;
}
void addLowercaseLetters(string& plaintext)
{
	char letter;
	fin >> letter;
	letter = tolower(letter);
	if (letter >= 'a' && letter <= 'z')
		plaintext.push_back(letter);
}
void fromLettersToNumbers(string& text, vector<unsigned int>& numbers)
{
	for (int i = 0; i < text.length(); i++)
		numbers.push_back(text[i] - 'a');
}
void fromNumbersToLetters(string& text, vector<unsigned int>& numbers)
{
	char c;
	for (int i = 0; i < numbers.size(); i++)
	{
		c = char(numbers[i] + 'a');
		text.push_back(c);
	}

}
void printKey(string& key)
{
	cout << "Length of the key is " << key.length() << ": ";
	for (int i = 0; i < key.length(); i++)
		cout << key[i];
	cout << '\n';

}
void printTransformedKey(int length, vector<unsigned int>& transformedKey)
{
	for (int i = 0; i < length; i++)
		cout << transformedKey[i] << ' ';
	cout << '\n';

}
void printText(string& ciphertext)
{
	for (int i = 0; i < ciphertext.length(); i++)
		cout << ciphertext[i];
	cout << '\n';
}
int main()
{
	long long start, finish;
	long long frequency = PerformanceFrequency();
	start = PerformanceCounter();
	string plaintext;
	//Remove all numbers, symbols and spaces from the plaintext
	fin.open("input.txt");
	addLowercaseLetters(plaintext); //Transforms input into a string that contains only lowercase letters and stores it in 'plaintext' string
	while (!fin.eof())	addLowercaseLetters(plaintext);
	cout << "Text has " << plaintext.length() << " characters." << endl;

	//Transform each letter into a number
	vector <unsigned int> transformedPlaintext;
	fromLettersToNumbers(plaintext, transformedPlaintext); //Transforms the text from letters to numbers and stores it in 'transformedPlaintext' vector

	//Generate randomly the key's length
	unsigned int keyLength;
	srand(time(NULL));
	keyLength = rand() % 26+1;

	//Generate randomly the key
	vector<unsigned int> transformedKey;
	unsigned int letter;
	for (int i = 0; i < keyLength; i++)
	{
		letter = rand() % 26;
		transformedKey.push_back(letter);
	}

	string key;
	fromNumbersToLetters(key, transformedKey);
	printKey(key);

	//Encrypt plaintext
	string ciphertext;
	for (int i = 0; i < transformedPlaintext.size(); i++)
	{
		char c = (transformedPlaintext[i] + transformedKey[i % keyLength]) % 26 + 'a';
		ciphertext.push_back(c);
	}

	vector<unsigned int>transformedCiphertext;
	fromLettersToNumbers(ciphertext, transformedCiphertext);
	cout << "Text encrypted with key:" << endl;
	printText(ciphertext);
	cout << endl;

	//Decrypt plaintext with key
	int k, c, m; // (m+k) mod 26 = c
	plaintext.clear();
	transformedPlaintext.clear();
	cout << "Plaintext deleted." << endl;
	for (int i = 0; i < ciphertext.length(); i++)
	{
		k = transformedKey[i % keyLength];
		c = transformedCiphertext[i];
		if (c < k) //if the letter of the ciphertext is smaller than the correspondent letter of the key => m+k is grater than 26
			m = 26 + c - k;
		else //otherwise, m+k is less than 26
			m = c - k;
		transformedPlaintext.push_back(m);
	}
	fromNumbersToLetters(plaintext, transformedPlaintext);
	cout << "Text decrypted with key:" << endl;
	printText(plaintext);

	//Find the length of the key
	plaintext.clear();
	transformedPlaintext.clear();
	key.clear();
	transformedKey.clear();
	keyLength = 0;
	cout << "\n\nPlaintext deleted.\nKey deleted\n";

	//compute the index of coincidence for each substring
	keyLength = 2; //first we suppose the key has length 2
	vector<double> IC; //here we will store the index of coincidence for each substring
	unsigned int fi; //number of apparitions of each character
	unsigned int alpha; //substring length
	int supposedKeyLength = 0;
	vector<unsigned int>letterFrequency;

	while (keyLength <= 26 )
	{
		IC.clear();
		for (int i = 0; i < keyLength; i++) //position of first letter of each substring
		{
			double sum = 0;
			letterFrequency.clear();
			letterFrequency.resize(26, 0);
			alpha = 0;
			for (int j = i; j < ciphertext.length(); j += keyLength) //search for the frequency of each letter in the entire substring
			{
				alpha++;
				letterFrequency[transformedCiphertext[j]]++; //Increase the frequency of each letter
			}
			//calculate the index of coincidence of the current substring
			for (int j = 0; j < 26; j++)
			{
				fi = letterFrequency[j];
				sum = sum + (double)((double)fi / alpha) * ((double)(fi - 1) / (alpha - 1));
			}

			IC.push_back(sum);

		}

		double ma = 0;
		for (int i = 0; i < IC.size(); i++)
			ma += IC[i];

		ma /= IC.size();
		if (abs(IC_EN - ma) < 0.01)
		{
			supposedKeyLength = keyLength;
			/*cout << "The key could be " << supposedKeyLength <<" with an ma of "<<ma<< " and with an IC of:" << endl;
			for (int i = 0; i < IC.size(); i++)
				cout << IC[i] << ' ';
			cout << endl;*/
			break;
		}
		keyLength++;

	}

	cout << "The key length found is " << supposedKeyLength << endl;

	//find the key knowing it's length
	int s,kj;
	double MIC=0;
	vector<double>probabilities = { 8.4966,2.0720,4.5388,3.3844,11.1607,1.8121,2.4705,3.0034,7.5448,0.1965,1.1016,5.4893,3.0129,6.6544,7.1635,3.1671,0.1962,7.5809,5.7351,6.9509,3.6308,1.0074,1.2899,0.2902,1.7779,0.2722 };
	for (int i = 0; i < keyLength; i++)
	{
		s = -1;
		MIC = 0;
		while (abs(IC_EN-MIC)>0.01)
		{
			s++;
			//compute the MIC
			MIC = 0;
			letterFrequency.clear();
			letterFrequency.resize(26, 0);
			alpha = 0;
			for (int j = i; j < ciphertext.length(); j += keyLength)
			{
				alpha++;
				letterFrequency[(transformedCiphertext[j] + s)%26]++;

			}

			for (int i = 0; i < 26; i++)
				MIC = MIC + (double)(probabilities[i]/100 * (double)letterFrequency[i] / alpha);
			
		}
		kj = (26 - s) % 26;
		transformedKey.push_back(kj);

	}
	
	fromNumbersToLetters(key, transformedKey);
	cout << "Key found:";
	printText(key);




	finish = PerformanceCounter();
	double elapsedseconds = (double)(finish - start) / frequency;
	cout << "Execution time: " << elapsedseconds << "s";


}
