//
//  main.cpp
//  TSP
//
//  Created by Hayden Scott on 1/22/16.
//  Copyright © 2016 Hayden Scott. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <sstream>
#include <ctime>
#include <chrono>
#include <random>

using namespace std;


double distanceBetweenCities(pair<double, double>, pair<double, double>);
vector<double> exhaustiveSearch(vector<int>& cities, vector< vector<double>>& cityDistances);
vector<pair<double, double>> fileLoader(ifstream &fin);
vector<pair<double, double>> randomPointGenerator(int numCities);
vector<double> nearestNeighborSearch(vector<int> citiesCopy, vector<vector<double>> cityDistances);
default_random_engine RNG(chrono::system_clock::now().time_since_epoch().count());
void writePointsToFile(vector<pair<double, double>>& cityLocations, double time);

int main() {
    vector<int> cities;
    vector< vector<double> > cityDistances;
    vector<pair<double, double>> cityLocations;
    
    cout << "clocks resolution: 1000000" << endl;
    
    
    int numCities = 0;
    //int numRuns = 0;
    void menu();
    cout << "TSP path finder" << endl;
    cout << "enter in filename or '1' to run a random sample" << endl;
    cout << "Selection: ";
    string selection;
    getline(cin, selection);
    
    // load points
    if (selection == "1") {
        cout << "Please enter number of points to traverse: ";
        cin >> numCities;
        cin.ignore();
        cityLocations = randomPointGenerator(numCities);
    }
    else {
        ifstream fin;
        fin.open(selection);
        if (fin.is_open()) {
            fin >> numCities;
            cityLocations = fileLoader(fin);
        }
        else {
            cout << "Error opening " << selection << " exiting" << endl;
            return -1;
        }
    }
    
    //resize vectors
    cityDistances.resize(numCities, vector<double>(numCities,0.0));
    for (int i = 0; i < numCities; i++) {
        cities.push_back(i);
    }
    
    //create 2x2 vector of distances between cities
    for (int i = 0; i < cityLocations.size(); i++) {
        for (int j = 0; j < cityLocations.size(); j++) {
            double dist =  distanceBetweenCities(cityLocations[i], cityLocations[j]);
            cityDistances[i][j] = dist;
        }
    }
    
    cout << "Run Exhaustive Search (1), Nearest neighbor (2), or both (3): ";
    int numSelection = 0;
    cin >> numSelection;
    cin.ignore();
    cout << endl;
    vector<double> EStimes;
    vector<double> NNtimes;
    switch (numSelection) {
        case 1:
            EStimes = exhaustiveSearch(cities, cityDistances);
            break;
        case 2:
            NNtimes = nearestNeighborSearch(cities, cityDistances);
            break;
        default:
            EStimes = exhaustiveSearch(cities, cityDistances);
            NNtimes = nearestNeighborSearch(cities, cityDistances);
            break;
    }
    cout << "****** Average times for " << numCities << " points.*********" << endl;
    double average = 0;
    if (!EStimes.empty()) {
        for (int i = 0; i < EStimes.size(); i++) {
            average += EStimes[i];
        }
        cout << "Exhaustive Search: " << average/EStimes.size() << "seconds." << endl;
    }
    if (!NNtimes.empty()) {
        average = 0;
        for (int i = 0; i < NNtimes.size(); i++) {
            average += NNtimes[i];
        }
        cout << "Nearest Neighbor: " << average/NNtimes.size() << " seconds." << endl;
    }
    
    cout << "Write locations to file?" << endl;
    cout << "(y/n): ";
    char write;
    cin >> write;
    cin.ignore();
    if (write == 'y') {
        writePointsToFile(cityLocations, average);
    }
        return 0;
}

vector<pair<double, double>> randomPointGenerator(int numCities){
    vector<pair<double, double>> cityLocations;
    uniform_int_distribution<double> distribution(0, 100000);
    for (int i = 0; i < numCities; i ++) {
        pair<double, double> temp(distribution(RNG), distribution(RNG));
        cityLocations.push_back(temp);
    }
    return cityLocations;
}

vector<pair<double, double>> fileLoader(ifstream &fin){
    vector<pair<double, double>> cityLocations;
    if (fin.is_open()) {
        while (fin) {
            string temp;
            getline(fin, temp);
            if (!temp.empty()) {
                istringstream nums(temp);
                double x = 0;
                double y = 0;
                nums >> x;
                nums >> y;
                pair<double, double> tempPair(x,y);
                cityLocations.push_back(tempPair);
            }
        }
    }
    return cityLocations;
}

void writePointsToFile(vector<pair<double, double>>& cityLocations, double time) {
    ofstream fout;
    string fileout;
    fileout = "randomCityLocations_time:" + to_string(time) + ".txt";
    fout.open(fileout);
    if (fout.is_open()) {
        fout << cityLocations.size();
        for (int i = 0; i < cityLocations.size(); i++) {
            fout << cityLocations[i].first << " " << cityLocations[i].second << endl;
        }
    }
    else {
        cout << "error, could not open " << fileout << endl;
    }
}

double distanceBetweenCities(pair<double, double> p1, pair<double, double> p2){
    return sqrt(pow((p1.first-p2.first),2)+pow((p1.second-p2.second),2));
}

vector<double> exhaustiveSearch(vector<int>& cities, vector< vector<double>>& cityDistances){
    double minDistance = numeric_limits<double>::max();
    vector<int> bestCityArrangement;
    
    int numRuns = 0;
    
    cout << "Enter number of times to rub this configuration with EXHAUSTIVE SEARCH: ";
    cin >> numRuns;
    vector<double> EStimes;
    for (int i = 0; i < numRuns; i++) {
        sort(cities.begin(), cities.end());
        
        clock_t begin = clock();
        while (next_permutation(cities.begin(),cities.end())) {
            double distance = 0;
            for (int i = 0; i < cities.size()-1; i++) {
                distance += cityDistances[cities[i]][cities[i+1]];
            }
            distance += cityDistances[cities.size()-1][0];
            if (distance < minDistance) {
                minDistance = distance;
                bestCityArrangement = cities;
            }

        }
        
        clock_t end = clock();
        double elapsedSecs = double(end - begin) / CLOCKS_PER_SEC;
        cout <<  endl << "min distance: " << minDistance << endl;
        cout << "Best city arrangement: ";
        for (int i = 0; i < bestCityArrangement.size(); i++) {
            cout << bestCityArrangement[i] << " ";
        }
        cout << "\ntime elapsed: " << elapsedSecs << " seconds\n" << endl;
        EStimes.push_back(elapsedSecs);
    }
    return EStimes;
}

vector<double> nearestNeighborSearch(vector<int> cities, vector<vector<double>> cityDistances){
    
    cout << "Enter number of times to run this configuration with NEAREST NEIGHBOR: ";
    int numRuns = 0;
    cin >> numRuns;
    cin.ignore();
    vector<double> NNtimes;
    
    for (int i = 0; i < numRuns; i++) {
        int currentCity = 0;
        int nextCity = -1;
        double minDistance = numeric_limits<double>::max();
        double shortestPath = 0;
        vector<int> bestCityArragnement;
        vector<int> citiesCopy = cities;
        
        clock_t begin = clock();
        while (citiesCopy.size() > 1) {
            minDistance = numeric_limits<double>::max();
            citiesCopy.erase(remove(citiesCopy.begin(), citiesCopy.end(), currentCity));
            
            bestCityArragnement.push_back(currentCity);
            for (int k = 0; k < citiesCopy.size(); k++) {
                if (cityDistances[currentCity][k] < minDistance) {
                    minDistance = cityDistances[currentCity][citiesCopy[k]];
                    nextCity = citiesCopy[k];
                }
            }
            shortestPath += minDistance;
            currentCity = nextCity;
        }
        shortestPath += cityDistances[currentCity][bestCityArragnement[0]];
        bestCityArragnement.push_back(currentCity);
        clock_t end = clock();
        
        double clicks = double(end - begin);
        double elapsedSecs = double(end - begin) / CLOCKS_PER_SEC;
        cout << endl << "min distance: " << shortestPath << endl;
        cout << "Best city arrangement: ";
        for (int i = 0; i < bestCityArragnement.size(); i++) {
            cout << bestCityArragnement[i] << " ";
        }
        cout << "clicks elapsed: " << clicks << endl;
        cout << endl << "time elapsed: " << elapsedSecs << " seconds" << endl << endl;
        NNtimes.push_back(elapsedSecs);
    }
    return NNtimes;
}