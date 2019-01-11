
/*ZAPISOVÁTKO MORSEOVY ABECEDY
 Využívá 3 tlačíka (tečka, čárka, oddělovač)
 Zapisuje na LCD 
 https://github.com/tesar-tech/arduino_and_morse_code
*/
#include <Wire.h> // pro práci s I2C
#include <LiquidCrystal_I2C.h> // pro práci s displayem


#define CARKABUTTONPIN 12 
#define TECKABUTTONPIN 8
#define ODDELBUTTONPIN 10
#define DISPLAY_NUMOFCOLUMNS 16 //Pracuji s displayem 16x2 




int carkaButtonState = 0;
int carkaButtonLastState = 0;
int teckaButtonState = 0;
int teckaButtonLastState = 0;
int oddelButtonState = 0;
int oddelButtonLastState = 0;
String tonesBuffer;
String text;
String expectedText;

String symbolsAlphabet[][2] =
{
	{ ".-","A" },
	{ "-...","B" },
	{ "-.-.","C" },
	{ "-..","D" },
	{ ".","E" },
	{ "..-.","F" },
	{ "--.","G" },
	{ "....","H" },
	{ "..","I" },
	{ ".---","J" },
	{ "-.-","K" },
	{ ".-..","L" },
	{ "--","M" },
	{ "-.","N" },
	{ "---","O" },
	{ ".--.","P" },
	{ "--.-","Q" },
	{ ".-.","R" },
	{ "...","S" },
	{ "-","T" },
	{ "..-","U" },
	{ "...-","V" },
	{ ".--","W" },
	{ "-..-","X" },
	{ "-.--","Y" },
	{ "--..","Z" },
	{ ".----","1" },
	{ "..---","2" },
	{ "...--","3" },
	{ "....-","4" },
	{ ".....","5" },
	{ "-....","6" },
	{ "--...","7" },
	{ "---..","8" },
	{ "----.","9" },
	{ "-----","0"}
};

LiquidCrystal_I2C lcd(0x27, DISPLAY_NUMOFCOLUMNS, 2);
char getToneFromButtonStates()
{
	//vrátí v případě uvolnění tlačítka
	//tedy když nynější stav je 0, předchozí 1

	if (!carkaButtonState&& carkaButtonLastState)
		return '-';
	if (!teckaButtonState && teckaButtonLastState)
		return '.';
	if (!oddelButtonState && oddelButtonLastState)
		return ' ';

	return (char)0;

}


char getSymbolFromBuffer()
{
	if (tonesBuffer == "")
		return ' '; //udělá mezeru, pokud předím nejsou žádné znaky

	for (int i = 0; i < sizeof symbolsAlphabet / sizeof symbolsAlphabet[0]; i++)
		//Projdu všechny symboly a porovnávám buffer s abecedou 
		if (tonesBuffer == symbolsAlphabet[i][0])
			return symbolsAlphabet[i][1][0];//pokud se rovna vrátím daný symbol

	//Buffer neodpovídá žádnému symbolu, pošlu tedy nic
	return (char)0;
}

void extractActionFromTonesBuffer()
{
	if (tonesBuffer == "......")//6x tečka
		text.remove(text.length() - 1, 1);//umaže jedno písmeno
	if (tonesBuffer == "------")//6x čárka
		text = "";//smaže celý text
}


void setup() {

	lcd.init();
	lcd.backlight();
	lcd.print("Morseovkovnitko");
	lcd.setCursor(0, 1);
	lcd.print("6x.Smaze1 6x-Vse");

	pinMode(CARKABUTTONPIN, INPUT);
	pinMode(TECKABUTTONPIN, INPUT);
	pinMode(ODDELBUTTONPIN, INPUT);
}

void loop() {

	//načtení stavů tlačítek
	carkaButtonState = digitalRead(CARKABUTTONPIN);
	teckaButtonState = digitalRead(TECKABUTTONPIN);
	oddelButtonState = digitalRead(ODDELBUTTONPIN);

	char tone = getToneFromButtonStates();//zjistí jestli a jaké tlačítko je stisklé

	if (tone != (char)0)
	{
		if (tone == ' ')//ukončuji sekvenci tónů, hledám symbol
		{
			char symbol = getSymbolFromBuffer();

			if (symbol != (char)0)//Sled tónů nalezl nějaký symbol, zapíše se do textu
			{
				text += symbol;
				if (text.length() > DISPLAY_NUMOFCOLUMNS)//Pokud přesáhne počet znaků velikost displaye,
					//napíše se nový znak na první místo. Ostatní se vymažou.
				{
					text = (String)symbol;
				}
			}
			else//Sled tónů nedává žádný symbol, ale možná nějakou akci (například vymazání znaku)
			{
				extractActionFromTonesBuffer();
			}
			tonesBuffer = "";//vymaže se buffer (čárky a tečky)
		}
		else//čárka nebo tečka
		{
			tonesBuffer += tone;
			if (tonesBuffer.length() > DISPLAY_NUMOFCOLUMNS)//pokud je tónů víc než velikost displaye, vymže se buffer
			{
				tonesBuffer = (String)tone;
			}
		}

		//psaní na display se provádí pouze v případě, že bylo zmačknuté nějaké tlačítko

		lcd.clear();//vyčistí se display
		lcd.print(text);//napíše se text
		lcd.setCursor(0, 1);
		lcd.print(tonesBuffer);//napíše se sled tónů

	}

	//aktualizuje se předchozí stav
	carkaButtonLastState = carkaButtonState;
	teckaButtonLastState = teckaButtonState;
	oddelButtonLastState = oddelButtonState;

}
