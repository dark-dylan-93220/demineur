// PROGRAMMATION IMPERATIVE #1 - Minesweeper //
// Crée le 15/10/2024 par Dylan HOLLEMAERT //
// Règles et spécificités vues sur https://minesweeper.online/fr/

#include <iostream> // Utilisé pour afficher du texte et prendre en compte les inputs utilisateur
#include <string>  // Utilisé pour traiter des strings de différentes façons
#include <cmath>  // Utilisé pour ceil() et pow()

using namespace std;

// Déclaration des fonctions, j'aurai préféré tout mettre dans un fichier.h (header)
bool isElementAbsentChar(char arr[], int size, char target);
bool areNumbersWrong(int arr[], int size, string target);
void GameSetup(int& boardDimensions, int& numberOfBombs, int& chosenMod, string& definedMod, string*& twoDBoard, int*& bombPositionsList, bool& hackMode, int& numberOfFlags, bool*& bombsListTrue, bool& isGameOver, bool& infiniteModEnabled, bool& wonInfiniteMod, bool& lostInfiniteMod, int& infiniteModIndex, int& moveNumber);
void Draw(int& dimensions, int& bombs, int& numberOfFlags, string*& twoDBoard, bool& hackMode, int& moveNumber, bool& isGameOver, bool& infiniteModEnabled, bool& wonInfiniteMod, bool& lostInfiniteMod, string& definedMod);
void Input(int*& legalMoves, int& boardDimensions, char*& lettersList, int*& numbersList, int& playerMove, string*& twoDBoard, int& moveNumber, int& numberOfFlags, bool& hackMode, int& numberOfBombs, bool& isGameOver, bool& infiniteModEnabled, bool& wonInfiniteMod, bool& lostInfiniteMod, string& definedMod);
void Logic(int& playerMove, bool& isGameOver, string*& twoDBoard, int*& bombPositionsList, int& moveNumber, int& boardDimensions, int*& squaresToLookFor, bool& hackMode, bool*& bombsListTrue, int& numberOfBombs, bool& infiniteModEnabled, bool& wonInfiniteMod, bool& lostInfiniteMod);
void revealSquares(int& playerMove, string*& twoDBoard, int*& bombPositionsList, int& moveNumber, int& boardDimensions, int*& squaresToLookFor, bool& hackMode, bool*& bombsListTrue, int& numberOfBombs);
void endMessage(string*& twoDBoard, int*& bombPositionsList, int*& safeSquares, int*& numbersList, int*& legalMoves, int*& squaresToLookFor, bool*& bombsListTrue, bool& hackMode, bool& infiniteModEnabled, bool& wonInfiniteMod, bool& lostInfiniteMod, bool& isGameOver, int& boardDimensions, int& numberOfBombs, int& chosenMod, string& definedMod, int& numberOfFlags, int& infiniteModIndex, int& moveNumber);

string asciiArt = R"(
 _____     ______     __    __     __     __   __     ______     __  __     ______    
/\  __-.  /\  ___\   /\ "-./  \   /\ \   /\ "-.\ \   /\  ___\   /\ \/\ \   /\  == \   
\ \ \/\ \ \ \  __\   \ \ \-./\ \  \ \ \  \ \ \-.  \  \ \  __\   \ \ \_\ \  \ \  __<   
 \ \____-  \ \_____\  \ \_\ \ \_\  \ \_\  \ \_\\"\_\  \ \_____\  \ \_____\  \ \_\ \_\ 
  \/____/   \/_____/   \/_/  \/_/   \/_/   \/_/ \/_/   \/_____/   \/_____/   \/_/ /_/ 
                                                                                      )";

const int BAD_BONUS = 3;   // Ajoute 3 bombes
const int GOOD_BONUS = -3; // Enlève 3 bombes
const int BONUS_UNSEEN_SQUARES_LIMIT = 10;
const int INFINITE_MOD_DIMESIONS[4] = { 4,8,16,32 };

int iterations = 0;

int main() {
    srand(time(NULL)); // Pour s'assurer de la génération random des bombes

    // Variables qui servent d'arguments de fonctions dans le code
    string modString, userInput;
    string* twoDimensionalBoard(nullptr);
    bool gameOver(false), hack(false), isInfiniteModEnabled(false), wonInInfiniteMod(false), lostInInfiniteMod(false);
    int dimensions(0), bombs(0), mod(0), playerMove(0), movesPlayed(0), flags(0), infiniteIndex(0);
    int* bombsPositionsList(nullptr), * squaresSafe(nullptr), * numbersList(nullptr), * legalMoves(nullptr), * searchingSquares(nullptr);
    char* lettersList(nullptr);
    bool* bombsList(nullptr);

    system("cls");

    // Choix utilisateur du mode débug
    cout << "Debug mode ? [y/N] > ";
    cin >> userInput;
    cout << endl;
    while (cin.fail() || (userInput != "y" && userInput != "N")) {
        cin.clear();
        cin.ignore(9999, '\n');
        cout << "Debug mode ? [y/N] > ";
        cin >> userInput;
        cout << endl;
    }
    if (userInput == "y") { hack = true; }
    if (!hack) { system("cls"); }

    // Driver code
    GameSetup(dimensions, bombs, mod, modString, twoDimensionalBoard, bombsPositionsList, hack, flags, bombsList, gameOver, isInfiniteModEnabled, wonInInfiniteMod, lostInInfiniteMod, infiniteIndex, movesPlayed);
    while (true) {
        Draw(dimensions, bombs, flags, twoDimensionalBoard, hack, movesPlayed, gameOver, isInfiniteModEnabled, wonInInfiniteMod, lostInInfiniteMod, modString);
        Input(legalMoves, dimensions, lettersList, numbersList, playerMove, twoDimensionalBoard, movesPlayed, flags, hack, bombs, gameOver, isInfiniteModEnabled, wonInInfiniteMod, lostInInfiniteMod, modString);
        Logic(playerMove, gameOver, twoDimensionalBoard, bombsPositionsList, movesPlayed, dimensions, searchingSquares, hack, bombsList, bombs, isInfiniteModEnabled, wonInInfiniteMod, lostInInfiniteMod);
        if (gameOver || wonInInfiniteMod || lostInInfiniteMod) { 
            endMessage(twoDimensionalBoard, bombsPositionsList, squaresSafe, numbersList, legalMoves, searchingSquares, bombsList, hack, isInfiniteModEnabled, wonInInfiniteMod, lostInInfiniteMod, gameOver, dimensions, bombs, mod, modString, flags, infiniteIndex, movesPlayed); 
            if (gameOver) { break; }
        }
    }
    return 0;
}

bool isElementAbsentChar(char arr[], int size, char target) {
    for (int i = 0; i < size; ++i) {
        if (arr[i] == target) { return false; }
    }
    cout << endl << "   - La lettre proposee n'est pas presente dans la grille (" << target << ")." << endl;
    return true;
}

bool areNumbersWrong(int arr[], int size, string target) {
    int intTarget;
    try {
        intTarget = stoi(target);
        if (intTarget <= 0 || intTarget > size) {
            cout << endl << "   - Ligne proposee hors de champ (" << target << ")." << endl;
            return true;
        }
        return false;
    }
    catch (exception) {
        cout << endl << "   - Ligne non valide (" << target << ")." << endl;
        return true;
    }
}

void GameSetup(int& boardDimensions, int& numberOfBombs, int& chosenMod, string& definedMod, string*& twoDBoard, int*& bombPositionsList, bool& hackMode, int& numberOfFlags, bool*& bombsListTrue, bool& isGameOver, bool& infiniteModEnabled, bool& wonInfiniteMod, bool& lostInfiniteMod, int& infiniteModIndex, int& moveNumber) {
    const string strGameRules = "Les regles du demineur sont tres simples : \n   - Le tableau est divise en cases, avec des mines placees au hasard. \n   - Pour gagner, vous devez ouvrir toutes les cases qui ne contiennent pas de mines. \n   - Le nombre sur une case indique le nombre de mines qui la touchent. \n   - Avec cette information, vous pouvez determiner les cases sures et les cases contenant des mines. \n   - Une case supposee contenir des mines peut etre marquee avec un drapeau.";
    const string strGameModes = "Dans le jeu, il y a plusieurs modes de difficultes :\n\n   1. Facile\n     - Grille de 9x9\n     - 7% de bombes\n\n   2. Intermediaire\n     - Grille de 15x15\n     - 12% de bombes\n\n   3. Expert\n     - Grille de 26x26\n     - 25% de bombes\n\n   4. Personnalisable\n     - Entre 4x4 et 52x52\n     - Entre 10 et 40% de bombes\n\n   5. Mode infini\n     - Le mode debute avec une grille de 4x4 et se termine avec une grille de 32x32\n     - A chaque victoire, la grille double de taille, jusqu'a atteindre 32x32\n       - Si vous gagnez en 32x32, alors vous remportez la victoire sur le mode infini\n     - A chaque defaite, la grille divise sa taille par 2 de taille jusqu'a atteindre 4x4\n       - Si vous perdez en 4x4, alors vous echouez le mode infini";

    float customBombPercentage;

    const int builtInBoardDimensions[3] = { 9, 15, 26 };
    const float levelsBombPercentages[3] = { 0.07F, 0.12F, 0.25F };

    const int MIN_BOARD_DIMENSIONS = 4;
    const int MAX_BOARD_DIMENSIONS = 52;

    const int MIN_BOMB_PERCENTAGE = 10;
    const int MAX_BOMB_PERCENTAGE = 40;

    if (!hackMode) {
        system("cls");
    }

    cout << asciiArt << endl;

    if (infiniteModEnabled == false) {
        cout << "Bienvenue dans le demineur!" << endl;
        cout << endl << strGameRules << endl;
        cout << endl << strGameModes << endl;

        cout << endl << "Veuillez choisir votre mode de jeu > ";
        cin >> chosenMod;
        while (cin.fail() || (chosenMod < 1 || chosenMod > 5)) {
            cin.clear();
            cin.ignore(9999, '\n');
            cout << endl << "Veuillez retaper votre choix (de 1 a 5) > ";
            cin >> chosenMod;
        }

        switch (chosenMod) {
        case 1:
            definedMod = "DEBUTANT";
            boardDimensions = builtInBoardDimensions[0];
            numberOfBombs = ceil(pow(boardDimensions, 2) * levelsBombPercentages[0]);
            break;
        case 2:
            definedMod = "INTERMEDIAIRE";
            boardDimensions = builtInBoardDimensions[1];
            numberOfBombs = ceil(pow(boardDimensions, 2) * levelsBombPercentages[1]);
            break;
        case 3:
            definedMod = "EXPERT";
            boardDimensions = builtInBoardDimensions[2];
            numberOfBombs = ceil(pow(boardDimensions, 2) * levelsBombPercentages[2]);
            break;
        case 4:
            definedMod = "CUSTOM";
            break;
        case 5:
            definedMod = "INFINI";
            infiniteModEnabled = true;
            infiniteModIndex = 0;
            break;
        default:
            break;
        }
    }
    else { definedMod = "INFINI"; }

    // Mode custom
    if (definedMod == "CUSTOM") {
        cout << endl << "Veuillez choisir les dimensions (en blocs) de votre grille (min. 4 | max. 52) > ";
        cin >> boardDimensions;
        while (cin.fail() || (boardDimensions < MIN_BOARD_DIMENSIONS || boardDimensions > MAX_BOARD_DIMENSIONS)) {
            cin.clear();
            cin.ignore(9999, '\n');
            cout << endl << "Veuillez retaper votre taille de grille (en blocs, entre 4 et 52) > ";
            cin >> boardDimensions;
        }
        cout << endl << "Veuillez choisir le pourcentage de bombes presents dans votre grille (min. 10 | max. 40) > ";
        cin >> customBombPercentage;
        while (cin.fail() || (customBombPercentage < MIN_BOMB_PERCENTAGE || customBombPercentage > MAX_BOMB_PERCENTAGE)) {
            cin.clear();
            cin.ignore(9999, '\n');
            cout << endl << "Veuillez retaper votre pourcentage de bombes (entre 10 et 40) > ";
            cin >> customBombPercentage;
        }
        numberOfBombs = ceil(pow(boardDimensions, 2) * (customBombPercentage / 100.f));
    }
    // Mode infini
    if (definedMod == "INFINI") {
        float infiniteModBombPercentage = 0.15f;
        if (wonInfiniteMod) {
            infiniteModIndex++;
            wonInfiniteMod = false;
        }
        if (lostInfiniteMod) {
            infiniteModIndex--;
            lostInfiniteMod = false;
        }
        if (infiniteModIndex == 4 || infiniteModIndex < 0) {
            if (infiniteModIndex == 4) { cout << endl << "Incroyable performance de votre part! Vous avez battu le mode infini!" << endl; }
            else { cout << endl << "Pfff... apprends les regles avant de te lancer dans une telle aventure..." << endl; }
            isGameOver = true; return;
        }
        boardDimensions = INFINITE_MOD_DIMESIONS[infiniteModIndex];
        numberOfBombs = ceil(pow(boardDimensions, 2) * infiniteModBombPercentage);
        moveNumber = 0;
    }
    numberOfFlags = numberOfBombs;
}

void Draw(int& boardDimensions, int& numberOfBombs, int& numberOfFlags, string*& twoDBoard, bool& hackMode, int& moveNumber, bool& isGameOver, bool& infiniteModEnabled, bool& wonInfiniteMod, bool& lostInfiniteMod, string& definedMod) {
    if (isGameOver || wonInfiniteMod || lostInfiniteMod) { return; }

    string lettersList[52] = { "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
                               "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };
    
    int squareIndex = 0;
    if (!hackMode) {
        system("cls");
    }
    cout << asciiArt << endl;

    const int SQUARES = pow(boardDimensions, 2);

    if (moveNumber == 0) {
        twoDBoard = new string[SQUARES];
        for (int i = 0; i < SQUARES; ++i) {
            twoDBoard[i] = "#";
        }
    }

    cout << "  ";  for (int i = 0; i < boardDimensions; i++) { cout << "  " << lettersList[i]; }; cout << endl;
    cout << "   "; for (int i = 0; i < boardDimensions; i++) { cout << "___"; }; cout << endl;

    for (int rowIndex = 0; rowIndex < boardDimensions; rowIndex++) {
        for (int columnIndex = 0; columnIndex < boardDimensions; columnIndex++) {
            if (columnIndex == 0) {
                if (rowIndex < 9) { cout << (rowIndex + 1) << "  "; }
                else { cout << (rowIndex + 1) << " "; }
            }

            cout << "[" << twoDBoard[squareIndex] << "]"; // Généraion de la grille par blocks de forme "[_]"

            if (columnIndex == (boardDimensions - 1)) { cout << endl; }
            ++squareIndex;
        }
    }
    cout << "--- MODE " << definedMod << " ---";
    cout << endl << "Drapeaux poses : " << (numberOfBombs - numberOfFlags) << " | " << "Drapeaux restants : " << numberOfFlags << endl;
}

void Input(int*& legalMoves, int& boardDimensions, char*& lettersList, int*& numbersList, int& playerMove, string*& twoDBoard, int& moveNumber, int& numberOfFlags, bool& hackMode, int& numberOfBombs, bool& isGameOver, bool& infiniteModEnabled, bool& wonInfiniteMod, bool& lostInfiniteMod, string& definedMod) {
    if (isGameOver || wonInfiniteMod || lostInfiniteMod) { return; }
    
    char lettersListAll[52] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
                               ,'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
    lettersList = new char[boardDimensions]; numbersList = new int[boardDimensions];
    
    for (int i = 0; i < boardDimensions; ++i) { lettersList[i] = lettersListAll[i]; }
    for (int i = 0; i < boardDimensions; ++i) { numbersList[i] = (i + 1); }

    ++moveNumber;

    int playerIsAction(0);

    // Condition de victoire
    int counter = 0;
    for (int i = 0; i < (boardDimensions*boardDimensions); ++i) {
        if (twoDBoard[i] == "#") {
            ++counter;
        }
    }
    if (counter == 0 && (infiniteModEnabled == false)) {
        cout << endl << "Gagne! Bravo!" << endl;
        isGameOver = true;
        return;
    }
    if (counter == 0 && (infiniteModEnabled == true)) {
        cout << endl << "Bien joue!" << endl;
        wonInfiniteMod = true;
        return;
    }

    if (moveNumber > 1) {
        cout << endl << "Quelle action voulez-vous realiser :\n   - 1. Deminer une bombe\n   - 2. Poser un drapeau\n\nVotre action > ";
        cin >> playerIsAction;
        while (cin.fail() || (playerIsAction < 1 || playerIsAction > 2)) {
            cin.clear();
            cin.ignore(9999, '\n');
            cout << endl << "Veuillez retaper votre action (1 ou 2) > ";
            cin >> playerIsAction;
        }
    }
    int isInputCorrect(0);
    // Vérification de la validité de l'input
    do {
        if (isInputCorrect == 0) {
            cout << endl << "Quel coup voulez-vous jouer ? > ";
        }
        if (isInputCorrect == -1) {
            cout << endl << "Case non jouable, veuillez retaper votre coup > ";
        }
        string movePlayed;
        cin >> movePlayed;
        string numbersString = movePlayed.substr(1, 2);
        bool isLetterAbsent = isElementAbsentChar(lettersList, boardDimensions, movePlayed[0]);
        bool numbersWrong = areNumbersWrong(numbersList, boardDimensions, numbersString);

        while (cin.fail() || (movePlayed.size() < 2 || movePlayed.size() > 3) || (isLetterAbsent || numbersWrong)) {
            cin.clear();
            cin.ignore(9999, '\n');
            cout << endl << "Reecris ton coup (Syntaxe : <LETTRE><CHIFFRE> => Exemple : a3 ou b15) > ";
            cin >> movePlayed;
            numbersString = movePlayed.substr(1, 2);
            if (movePlayed.size() < 2) { cout << endl << "   - Input trop courte" << endl; }
            if (movePlayed.size() > 3) { cout << endl << "   - Input trop longue" << endl; }
            isLetterAbsent = isElementAbsentChar(lettersList, boardDimensions, movePlayed[0]);
            numbersWrong = areNumbersWrong(numbersList, boardDimensions, numbersString);
        }

        // CALCUL DE L'INDEX DE LA CASE

        int indexColumn(0), indexRow(0);
        switch (movePlayed[0]) {
        case 'a':indexColumn = 0;  break; case 'A':indexColumn = 26; break;
        case 'b':indexColumn = 1;  break; case 'B':indexColumn = 27; break;
        case 'c':indexColumn = 2;  break; case 'C':indexColumn = 28; break;
        case 'd':indexColumn = 3;  break; case 'D':indexColumn = 29; break;
        case 'e':indexColumn = 4;  break; case 'E':indexColumn = 30; break;
        case 'f':indexColumn = 5;  break; case 'F':indexColumn = 31; break;
        case 'g':indexColumn = 6;  break; case 'G':indexColumn = 32; break;
        case 'h':indexColumn = 7;  break; case 'H':indexColumn = 33; break;
        case 'i':indexColumn = 8;  break; case 'I':indexColumn = 34; break;
        case 'j':indexColumn = 9;  break; case 'J':indexColumn = 35; break;
        case 'k':indexColumn = 10; break; case 'K':indexColumn = 36; break;
        case 'l':indexColumn = 11; break; case 'L':indexColumn = 37; break;
        case 'm':indexColumn = 12; break; case 'M':indexColumn = 38; break;
        case 'n':indexColumn = 13; break; case 'N':indexColumn = 39; break;
        case 'o':indexColumn = 14; break; case 'O':indexColumn = 40; break;
        case 'p':indexColumn = 15; break; case 'P':indexColumn = 41; break;
        case 'q':indexColumn = 16; break; case 'Q':indexColumn = 42; break;
        case 'r':indexColumn = 17; break; case 'R':indexColumn = 43; break;
        case 's':indexColumn = 18; break; case 'S':indexColumn = 44; break;
        case 't':indexColumn = 19; break; case 'T':indexColumn = 45; break;
        case 'u':indexColumn = 20; break; case 'U':indexColumn = 46; break;
        case 'v':indexColumn = 21; break; case 'V':indexColumn = 47; break;
        case 'w':indexColumn = 22; break; case 'W':indexColumn = 48; break;
        case 'x':indexColumn = 23; break; case 'X':indexColumn = 49; break;
        case 'y':indexColumn = 24; break; case 'Y':indexColumn = 50; break;
        case 'z':indexColumn = 25; break; case 'Z':indexColumn = 51; break; default:break;
        }
        if (movePlayed.size() == 2) { indexRow = (stoi(movePlayed.substr(1, 1)) - 1); }
        else { indexRow = (stoi(movePlayed.substr(1, 2)) - 1); }

        playerMove = ((indexRow * boardDimensions) + indexColumn);

        if (twoDBoard[playerMove] == "#" || twoDBoard[playerMove] == "=" || twoDBoard[playerMove] == "~" || (twoDBoard[playerMove] == "F" && playerIsAction == 2) || (twoDBoard[playerMove] == "*" && hackMode == true)) {
            //                 CASE  ^^^              BONUS PAS COOL ^^^                  BONUS COOL ^^^                         FLAG ^^^                                                BOMBE ^^^ si jamais on est en mode débug
            isInputCorrect = 1;
        }
        else {
            isInputCorrect = -1;
        }
    } while (isInputCorrect <= 0);

    switch (playerIsAction) {
    case 1:
        break;
    case 2:
        if (numberOfFlags == 0 && twoDBoard[playerMove] == "F") {
            twoDBoard[playerMove] = "#";
            numberOfFlags += 1;
            return;
        }
        if (numberOfFlags > 0) { 
            if (twoDBoard[playerMove] == "F") {
                twoDBoard[playerMove] = "#";
                numberOfFlags += 1;
                playerMove = -1;
                return;
            }
            twoDBoard[playerMove] = "F"; 
            numberOfFlags -= 1; 
            Draw(boardDimensions, numberOfBombs, numberOfFlags, twoDBoard, hackMode, moveNumber, isGameOver, infiniteModEnabled, wonInfiniteMod, lostInfiniteMod, definedMod);
            Input(legalMoves, boardDimensions, lettersList, numbersList, playerMove, twoDBoard, moveNumber, numberOfFlags, hackMode, numberOfBombs, isGameOver, infiniteModEnabled, wonInfiniteMod, lostInfiniteMod, definedMod);
        }
        else {
            cout << endl << "Vous n'avez plus de drapaux." << endl;
            moveNumber -= 1; // On fait comme si le tour n'avais jamais existé, car inutile.
            return;
        }
        break;
    default:
        break;
    }
}

void Logic(int& playerMove, bool& isGameOver, string*& twoDBoard, int*& bombPositionsList, int& moveNumber, int& boardDimensions, int*& squaresToLookFor, bool& hackMode, bool*& bombsListTrue, int& numberOfBombs, bool& infiniteModEnabled, bool& wonInfiniteMod, bool& lostInfiniteMod) {
    if (isGameOver || wonInfiniteMod || lostInfiniteMod) { return; }
    if (playerMove == -1) { return; } // Le cas où un drapeau a été remplacé

    const int SQUARES = (pow(boardDimensions, 2));
    if (moveNumber == 1) {
        if (hackMode) {
            cout << endl << "   - [DEBUG] Emplacements des bombes dans bombPositionsList : ";
        }
        bombPositionsList = new int[numberOfBombs + BAD_BONUS];
        int numberOfElementsInBombPositionsList = 0;
        int numberOfSquares = pow(boardDimensions, 2);
        for (int i = 0; i < numberOfBombs; ++i) {
            int bomb = rand() % numberOfSquares;
            bombPositionsList[i] = bomb;
            for (int j = 0; j < numberOfElementsInBombPositionsList; ++j) {
                while (bombPositionsList[i] == bombPositionsList[j] || bombPositionsList[i] == playerMove || bombPositionsList[j] == playerMove) {
                    int bomb = rand() % numberOfSquares;
                    bombPositionsList[i] = bomb;
                }
            }
            ++numberOfElementsInBombPositionsList;
            cout << bombPositionsList[i] << " ";
        }
        if (hackMode) {
            for (int i = 0; i < numberOfElementsInBombPositionsList; i++) {
                int x = bombPositionsList[i];
                twoDBoard[x] = "*";
            }
            cout << endl << "   - [DEBUG] Coup numero : " << moveNumber;
        }
    }
    if (moveNumber > 1) {
        if (hackMode) {
            cout << endl << "   - [DEBUG] Coup numero : " << moveNumber << endl;
            cout << endl << "   - [DEBUG] Emplacements des bombes dans bombPositionsList : ";
        }
        for (int i = 0; i < numberOfBombs; ++i) {
            if (bombPositionsList[i] == playerMove && !infiniteModEnabled) {
                cout << endl << "Perdu!" << endl;
                isGameOver = true;
                return;
            }
            if (bombPositionsList[i] == playerMove && infiniteModEnabled) {
                cout << endl << "Dommage!" << endl;
                lostInfiniteMod = true;
                return;
            }
            if (hackMode) {
                cout << bombPositionsList[i] << " ";
            }
        }
        if (hackMode) {
            cout << endl;
        }
    }
    if (isGameOver || wonInfiniteMod || lostInfiniteMod) { return; }
    bombsListTrue = new bool[SQUARES];
    for (int i = 0; i < SQUARES; ++i) {
        bombsListTrue[i] = false;
    }
    revealSquares(playerMove, twoDBoard, bombPositionsList, moveNumber, boardDimensions, squaresToLookFor, hackMode, bombsListTrue, numberOfBombs);
    if (hackMode) {
        cout << endl << "Nombre d'iterations : " << iterations;
    }
}

void revealSquares(int& playerMove, string*& twoDBoard, int*& bombPositionsList, int& moveNumber, int& boardDimensions, int*& squaresToLookFor, bool& hackMode, bool*& bombsListTrue, int& numberOfBombs) {
    iterations++;
    const int SQUARES = boardDimensions * boardDimensions;
    int currentSquare, squareOnTop, squareOnBottom, squareToTheLeft, squareToTheRight, squareToTheTopLeft, squareToTheTopRight, squareToTheBottomLeft, squareToTheBottomRight;

    // Vérifications des limites de la récursion
    if (playerMove >= SQUARES || playerMove < 0 || bombsListTrue[playerMove] == true) { return; }
    bombsListTrue[playerMove] = true;
    twoDBoard[playerMove] = " ";      // Avec le stack de base, si la fonction est appellée plus de 5700 fois alors il y a stackOverflow

    int otherTemp(0);
    currentSquare = playerMove;
    squareOnTop = currentSquare - boardDimensions;
    squareOnBottom = currentSquare + boardDimensions;
    squareToTheLeft = currentSquare - 1;
    squareToTheRight = currentSquare + 1;
    squareToTheTopLeft = currentSquare - (boardDimensions + 1);
    squareToTheTopRight = currentSquare - (boardDimensions - 1);
    squareToTheBottomLeft = currentSquare + (boardDimensions - 1);
    squareToTheBottomRight = currentSquare + (boardDimensions + 1);

    int temp[9] = { currentSquare, squareToTheTopLeft, squareToTheLeft, squareToTheBottomLeft, squareToTheTopRight, squareToTheRight, squareToTheBottomRight, squareOnTop, squareOnBottom };
    for (int i = 0; i < boardDimensions; ++i) {
        for (int ii = 1; ii < 4; ++ii) { // CASES A GAUCHE - Problemes ici : multiples de (boardDimensions - 1)
            otherTemp = (temp[ii] - (i * boardDimensions));
            if ((otherTemp % (boardDimensions - 1) == 0) && (temp[ii] > 0) && ((otherTemp == boardDimensions - 1))) {
                temp[ii] = -1;
            }
        }
        for (int ii = 4; ii < 7; ++ii) { // CASES A DROITE - Problemes ici : multiples de boardDimensions
            if (temp[ii] == 0) { temp[ii] += boardDimensions; }
            otherTemp = (temp[ii] - (i * boardDimensions));
            if (otherTemp % boardDimensions == 0 && temp[ii] > 0 && otherTemp == boardDimensions) {
                temp[ii] = -1;
            }
        }
    }

    int numberOfBombsAroundTheSquare = 0;

    for (int i = 0; i < 9; ++i) {
        if (temp[i] < 0 || temp[i] >= SQUARES) { temp[i] = -1; }
        numberOfBombsAroundTheSquare = 0;
        for (int ii = 1; ii < 9; ++ii) {
            for (int iii = 0; iii < numberOfBombs; ++iii) {
                if (temp[ii] == bombPositionsList[iii]) { numberOfBombsAroundTheSquare++; }
            }
        }
    }

    if (numberOfBombsAroundTheSquare == 0) {
        int row = playerMove / boardDimensions;
        int col = playerMove % boardDimensions;

        // Vérifier les 8 voisins dans la grille linéaire (haut, bas, gauche, droite et diagonales)
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                // Ne rien faire si on est sur la case actuelle
                if (i == 0 && j == 0) continue;

                int newRow = row + i;
                int newCol = col + j;

                // Calculer le nouvel index
                int newIndex = newRow * boardDimensions + newCol;

                // Vérifier si le voisin est dans les limites de la grille
                if (newRow >= 0 && newRow < boardDimensions && newCol >= 0 && newCol < boardDimensions && newIndex >= 0 && newIndex < SQUARES) {
                    revealSquares(newIndex, twoDBoard, bombPositionsList, moveNumber, boardDimensions, squaresToLookFor, hackMode, bombsListTrue, numberOfBombs);
                }
            }
        }
    }

    else { // Si il y'a plus que 0 bombes autour d'une case, alors on affiche le nombre de bombes présentes autour de la case sur ladite case
        twoDBoard[playerMove] = to_string(numberOfBombsAroundTheSquare);
    }
}

void endMessage(string*& twoDBoard, int*& bombPositionsList, int*& safeSquares, int*& numbersList, int*& legalMoves, int*& squaresToLookFor, bool*& bombsListTrue, bool& hackMode, bool& infiniteModEnabled, bool& wonInfiniteMod, bool& lostInfiniteMod, bool& isGameOver, int& boardDimensions, int& numberOfBombs, int& chosenMod, string& definedMod, int& numberOfFlags, int& infiniteModIndex, int& moveNumber) {
    delete[] twoDBoard;
    delete[] bombPositionsList;
    delete[] safeSquares;
    delete[] numbersList;
    delete[] legalMoves;
    delete[] squaresToLookFor;
    delete[] bombsListTrue;
    if (wonInfiniteMod) {
        GameSetup(boardDimensions, numberOfBombs, chosenMod, definedMod, twoDBoard, bombPositionsList, hackMode, numberOfFlags, bombsListTrue, isGameOver, infiniteModEnabled, wonInfiniteMod, lostInfiniteMod, infiniteModIndex, moveNumber);
    }
    if (lostInfiniteMod) {
        GameSetup(boardDimensions, numberOfBombs, chosenMod, definedMod, twoDBoard, bombPositionsList, hackMode, numberOfFlags, bombsListTrue, isGameOver, infiniteModEnabled, wonInfiniteMod, lostInfiniteMod, infiniteModIndex, moveNumber);
    }
    if (isGameOver) {
        string userEntry;
        cout << endl << "Tu veux recommencer ? [y/N] > ";
        cin >> userEntry;
        while (cin.fail() || (userEntry != "y" && userEntry != "N")) {
            cin.clear();
            cin.ignore(9999, '\n');
            cout << endl << "Tu veux recommencer ? [y/N] > ";
        }
        if (userEntry == "y") { main(); }
    }
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage