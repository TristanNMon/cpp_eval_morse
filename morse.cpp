#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <cmath>
#include <unordered_map>

using namespace std;

//Définition des constantes utilisées pour le signal sonore 

const double frequency = 640;
const int sampleRate = 44100;
const double dot_length = 0.1 ;
const double entre_lettre_length = 0.1 ;
const double dash_length = 0.3 ;
const double space_length = 0.3 ;
const double slash_length = 0.3 ;
const double amplitude = 32760;

//Définition de la fonction créant l'entête 

void writeWavHeader(std::ofstream &output, int dataSize) {     
    output.write("RIFF", 4);    
    output.write(reinterpret_cast<const char*>(&dataSize), 4);     
    output.write("WAVE", 4);     
    output.write("fmt ", 4);     
    int subBlockSize = 16;
    output.write(reinterpret_cast<const char*>(&subBlockSize), 4);     
    short int audioFormat = 1;     
    output.write(reinterpret_cast<const char*>(&audioFormat), 2);     
    short int numChannels = 1;     
    output.write(reinterpret_cast<const char*>(&numChannels), 2);     
    int sampleRate = ::sampleRate;     
    output.write(reinterpret_cast<const char*>(&sampleRate), 4);     
    int byteRate = ::sampleRate * 2;     
    output.write(reinterpret_cast<const char*>(&byteRate), 4);     
    short int blockAlign = 2;     
    output.write(reinterpret_cast<const char*>(&blockAlign), 2);     
    short int bitsPerSample = 16;     
    output.write(reinterpret_cast<const char*>(&bitsPerSample), 2);     
    output.write("data", 4);     
    output.write(reinterpret_cast<const char*>(&dataSize), 4); 
} 

//Définition de la fonction décodant l'entête du fichier wav

void parseWAVHeader(ifstream &wavFile, int &sampleRate, int &numChannels, int &bitsPerSample , int &dataSize, int &bitsPerSecond) {

    // Définition des variables

    char chunkID[4];
    int chunkSize;
    char format[4];
    char subChunk1ID[4];
    int subChunk1Size;
    short audioFormat;
    short numChannelsShort;
    int sampleRateInt;
    int byteRate;
    short blockAlign;
    short bitsPerSampleShort;
    char data [4];
    int Data;

    // Lecture de l'entête

    wavFile.read(chunkID, 4);
    wavFile.read((char*)&chunkSize, 4);
    wavFile.read(format, 4);

    // Check if the file is a valid WAV file
    if (string(chunkID, 4) != "RIFF" || string(format, 4) != "WAVE") {
        cout << "Error: Not a valid WAV file" << endl;
        return;
    }

    wavFile.read(subChunk1ID, 4);
    wavFile.read((char*)&subChunk1Size, 4);
    wavFile.read((char*)&audioFormat, 2);
    wavFile.read((char*)&numChannelsShort, 2);
    wavFile.read((char*)&sampleRateInt, 4);
    wavFile.read((char*)&byteRate, 4);
    wavFile.read((char*)&blockAlign, 2);
    wavFile.read((char*)&bitsPerSampleShort, 2);
    wavFile.read(data, 4);
    wavFile.read((char*)&Data, 4);

    // Variables de sortie

    dataSize = chunkSize ;
    sampleRate = sampleRateInt;
    numChannels = numChannelsShort;
    bitsPerSample = bitsPerSampleShort;
    bitsPerSecond = byteRate;

}

// Définition de la fonction formant les signal sinusoidale à entrer dans le fichier wav

void writeSineWave(std::ofstream &output, double duration, double frequency, double amp) {

    int samples = int(sampleRate*duration);

    for (int i = 0; i < samples; i++) {

        double time = i / double(sampleRate);
        short int sample = amp * sin(2 * M_PI * frequency * time);
        output.write(reinterpret_cast<const char*>(&sample), 2);    
                 
    }
}

// Définition de la fonction décodant un message morse

std::string code_vers_lettre(std::string c, std::string entry){

    std::string exit;

    std::map <std::string, std::string> trad2 {{".-","a"},{"-...","b"},{"-.-.","c"},{"-..","d"},{".","e"},{"..-.","f"},{"--.","g"},{"....","h"},{"..","i"},{".---","j"},{"-.-","k"},{".-..","l"},{"--","m"},{"-.","n"},{"---","o"},{".--.","p"},{"--.-","q"},{".-.","r"},{"...","s"},{"-","t"},{"..-","u"},{"...-","v"},{".--","w"},{"-..-","x"},{"-.--","y"},{"--..","z"},{".----","1"},{"..---","2"},{"...--","3"},{"....-","4"},{".....","5"},{"-....","6"},{"--...","7"},{"---..","8"},{"---.","9"},{"-----","0"},{"/"," "}};
    
    exit = entry + trad2[c];

    return exit ;
}

// Définition de la fonction décodant un message en français

std::string code_vers_morse (std::string c, std::string entry){

    std::string exit;
    std::string new_word;
    std::map <std::string, std::string> trad1 {{"a",".-"},{"b","-..."},{"c","-.-."},{"d","-.."},{"e","."},{"f","..-."},{"g","--."},{"h","...."},{"i",".."},{"j",".---"},{"k","-.-"},{"l",".-.."},{"m","--"},{"n","-."},{"o","---"},{"p",".--."},{"q","--.-"},{"r",".-."},{"s","..."},{"t","-"},{"u","..-"},{"v","...-"},{"w",".--"},{"x","-..-"},{"y","-.--"},{"z","--.."},{"1",".----"},{"2","..---"},{"3","...--"},{"4","....-"},{"5","....."},{"6","-...."},{"7","--..."},{"8","---.."},{"9","---."},{"0","-----"},{" ","/"}};
    
    int k=c.length();

    for(int i=0; i<k ;i++){
            char let = c[i];
            std::string lettre;
            lettre.push_back(let);
            if (new_word.length() == 0){
                new_word = trad1[lettre];
            }
            else{
                new_word = new_word + " " + trad1[lettre];
            }    
    }   

    if (entry.length() == 0){ 
       exit = new_word;
    }
    else {
        exit = entry + " / " + new_word;
    }
    return exit;
}

// Définition de la fonction main qui prend en entrée soit l'adresse du fichier wav soit un message en morse ou français 

int main(int argc, char** argv){

    // Définition des variables de sortie

    std::string message_morse;
    std::string message_lettre;

    // Demande si l'on a entré un fichier wav ou un message text
    std::string decode;
    std::cout << "Enter wav or txt : ";
    std::cin >> decode;

    // Si la réponse est wav on passe au décodage du fichier 

    if (decode == "wav"){
        ifstream wavFile(argv[1], ios::binary);

    // Vérification si le fichier est bien wav

        if (!wavFile) {
            cout << "Error opening file" << endl;
            return 1;
        }

    // Définition des variables intéressant à récupérer de l'entête

        int sampleRate;
        int numChannels;
        int bitsPerSample;
        int dataSize;
        int bitsPerSecond;
    
    // utilisation de la fonction qui permet de décoder l'enête

        parseWAVHeader(wavFile, sampleRate, numChannels, bitsPerSample, dataSize, bitsPerSecond);

    // Determinermination de la duréer du fichier wav puis du nombre de points (samples) dans le fichier wav

        double duration = double (dataSize) / double (bitsPerSecond);
        int numSamples = duration * sampleRate;

    // Lecture des samples du fichier wav
    // on remarque en parcourant les samples pour les moments de pause entre les . et les - sont des 0 il suffit de calculer le nombre de sample = 0 puis la longueur de sample != 0
    // définition des variables nécessaire 

        int m = 0 ;
        int n = 0 ;
        short samples = 1 ;
        int samples1 = 1;
        int samples_1 = 2; 
        int samples_2 = 3;
        int samples_3 = 4;

    // Définition des variables qui prendront en compte la longueur des smaples des différents caractères

        int a = 0 , b = 0 , c = 0 , d = 0 , e = 0;

        std::unordered_map< int , std::string> list_decode;
        std::vector <int> list_long;
        std::vector <std::string> list_char {"." , "" , "-" , " " , " / "};
        std::vector <int> list_byte;

    // Lecture des samples

        for(int i =0 ;i < numSamples; i++){

            samples_3 = samples_2;
            samples_2 = samples_1;
            samples_1 = samples1;
            short samples;
            wavFile.read((char*)&samples, 2);
            samples1 = int(samples);

            // condition de si on passe d'un moment de silence ou de son

            if (((samples1 == 0) and (samples_1 == 0) and (samples_2 != 0) and (samples_3 != 0)) or ((samples1 != 0) and (samples_1 != 0) and (samples_2 == 0) and (samples_3 == 0))){
                n = m-2;
                list_byte.push_back(n);

                // rangement des longueurs des différentes variables en ordre croissance de longueurs 

                if (a == 0) {
                    a = n;
                    list_long.push_back(a);
                }
                else if ((a != n) and (b==0)){
                    b = n;
                    if (b > a){
                        list_long.push_back(b);
                    }
                    else{
                        list_long.pop_back();
                        list_long.push_back(b);
                        list_long.push_back(a);
                    }
                }
                else if ((a != n ) and (b!= n ) and (c==0)){
                    c = n;
                    if (c > list_long[1]){
                        list_long.push_back(c);
                    }
                    else if (c > list_long[0]){
                        int un = list_long.back();
                        list_long.pop_back();
                        list_long.push_back(c);
                        list_long.push_back(un);
                
                    }
                    else{
                        int un = list_long.back();
                        list_long.pop_back();
                        int deux = list_long.back();
                        list_long.pop_back();
                        list_long.push_back(c);
                        list_long.push_back(deux);
                        list_long.push_back(un);
                    }
                }
                else if ((a != n ) and (b!= n ) and (c!= n ) and (d==0)){
                    d = n;
                    if (d > list_long[2]){                    
                        list_long.push_back(d);                   
                    }
                    else if (d > list_long[1]){
                        int un = list_long.back();
                        list_long.pop_back();
                        list_long.push_back(d);
                        list_long.push_back(un);
                    }
                    else if (d > list_long[0]){
                        int un = list_long.back();
                        list_long.pop_back();
                        int deux = list_long.back();
                        list_long.pop_back();
                        list_long.push_back(d);
                        list_long.push_back(deux);
                        list_long.push_back(un);
                    }
                    else{
                        int un = list_long.back();
                        list_long.pop_back();
                        int deux = list_long.back();
                        list_long.pop_back();
                        int trois = list_long.back();
                        list_long.pop_back();
                        list_long.push_back(d);
                        list_long.push_back(trois);
                        list_long.push_back(deux);
                        list_long.push_back(un);
                    }
                }
                else if ((a != n ) and (b!= n ) and (c!= n ) and (d!= n ) and (e==0)){
                    e = n;
                    if (e > list_long[3]){                    
                        list_long.push_back(e);                    
                    }
                    else if (e > list_long[2]){
                        int un = list_long.back();
                        list_long.pop_back();
                        list_long.push_back(e);
                        list_long.push_back(un);
                
                    }
                    else if (e > list_long[1]){
                        int un = list_long.back();
                        list_long.pop_back();
                        int deux = list_long.back();
                        list_long.pop_back();
                        list_long.push_back(e);
                        list_long.push_back(deux);
                        list_long.push_back(un);
                    }
                    else if (e > list_long[0]){
                        int un = list_long.back();
                        list_long.pop_back();
                        int deux = list_long.back();
                        list_long.pop_back();
                        int trois = list_long.back();
                        list_long.pop_back();
                        list_long.push_back(e);
                        list_long.push_back(trois);
                        list_long.push_back(deux);
                        list_long.push_back(un);
                    }
                    else {
                        int un = list_long.back();
                        list_long.pop_back();
                        int deux = list_long.back();
                        list_long.pop_back();
                        int trois = list_long.back();
                        list_long.pop_back();
                        int quatre = list_long.back();
                        list_long.pop_back();
                        list_long.push_back(e);
                        list_long.push_back(quatre);
                        list_long.push_back(trois);
                        list_long.push_back(deux);
                        list_long.push_back(un);
                    }
                }
                m = 2; 
            }
            m = m + 1;
        }

        //association de chaque variable a b c d e à chaque caractère qui lui correspond 

        for (int h = 0; h < list_long.size(); h++){
            list_decode.insert({list_long[h], list_char[h]});
        }

        //traduction 

        std::string lettre;
        std::map <std::string, std::string> trad2 {{".-","a"},{"-...","b"},{"-.-.","c"},{"-..","d"},{".","e"},{"..-.","f"},{"--.","g"},{"....","h"},{"..","i"},{".---","j"},{"-.-","k"},{".-..","l"},{"--","m"},{"-.","n"},{"---","o"},{".--.","p"},{"--.-","q"},{".-.","r"},{"...","s"},{"-","t"},{"..-","u"},{"...-","v"},{".--","w"},{"-..-","x"},{"-.--","y"},{"--..","z"},{".----","1"},{"..---","2"},{"...--","3"},{"....-","4"},{".....","5"},{"-....","6"},{"--...","7"},{"---..","8"},{"---.","9"},{"-----","0"},{"/"," "}};

        for (int h = 0; h < list_byte.size(); h++){

            if (list_decode[list_byte[h]] == " "){
                message_lettre = message_lettre + trad2[lettre];
                lettre.clear();
            }
            else if (list_decode[list_byte[h]] == " / "){
                message_lettre = message_lettre + trad2[lettre] + " ";
                lettre.clear();
            }
            else{
            lettre = lettre + list_decode[list_byte[h]] ;
            }
            message_morse = message_morse + list_decode[list_byte[h]] ;
        }

        message_lettre = message_lettre + trad2[lettre];
    }

    // Passage à la partie d'encodage d'un message francais en morse ou morse en français
                
    else if( decode == "txt") {

        // Vérification si faut code un message du français au morse ou du morse en français

        std::string test=argv[1];
        char lettre = test[0];
        std::string test_lettre;
        test_lettre.push_back(lettre);

        // Définition du nom du fichier 

        std::string nom_fichier;

        // Test de si c'est un message en morse et décodage du message

        if ((test_lettre == "-") or (test_lettre == ".")){
            for (int i=1 ; i < argc ; i++){
            std::string Acoder = argv[i];
            message_lettre = code_vers_lettre(Acoder, message_lettre);
            }
        }

        // Codage du message en morse et création du fichier wav

        else{

            // Codage du message en morse    

            for (int i=1 ; i < argc ; i++){
                message_lettre = message_lettre + " " + argv[i];
                std::string Acoder = argv[i];
                message_morse = code_vers_morse(Acoder, message_morse);
            }

            // Choix du mon du fichier

            std::cout << "Choose a file name : ";
            std::cin >> nom_fichier;
            nom_fichier = nom_fichier + ".wav";
            
            // Détermination de la taille du fichier

            int dataSize = 0;
            for (auto c : message_morse) {
                if (c == '.') {
                    dataSize += dot_length * sampleRate * 4 ;
                } 
                else if (c == '-') {
                    dataSize += ( dash_length + dot_length )* sampleRate * 4 ;
                } 
                else if (c == ' ') {
                    dataSize += space_length * sampleRate * 2 ;
                }
                else if (c == '/') {
                        dataSize += space_length * sampleRate *2;
                }
            }

            // Création du fichier wav en binaire

            std::ofstream output(nom_fichier, std::ios::binary);

            // Création de l'entête d'un fichier wav 

            writeWavHeader(output, dataSize);

            // Ajout des données dans le fichier wav

            for (auto c : message_morse) {
                if (c == '.') {
                    writeSineWave(output, dot_length, frequency, amplitude);
                    writeSineWave(output, entre_lettre_length, frequency, 0);
                } 
                else if (c == '-') {
                    writeSineWave(output, dash_length, frequency, amplitude);
                    writeSineWave(output, entre_lettre_length, frequency, 0);
                } 
                else if (c == ' ') {
                    writeSineWave(output, space_length, frequency, 0);
                }
                else if (c == '/') {
                    writeSineWave(output, space_length , frequency, 0);
                }
            }

            // Fermeture du fichier wav

            output.close();
        }
    }

    // Sortie des messages

    std::cout << "Le message en morse est : " << message_morse << std::endl;
    std::cout << "Le message en francais ou anglais est : " << message_lettre << std::endl; 
    return 0;
}