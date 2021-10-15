#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <cstdlib>
#include <map>
#include <mutex>
#include <vector>
#include <chrono>
/*
 * Date: 5/4/2020
 * This project simulates a mail in election reporting cost, winner per state, and winner of the electoral college.
 * There should be no issues everything compiles and runs smoothly.
 *
 *
 */
using namespace std;
const int full=50; //All of the states
//default set to 100 percent of people voting
double voters=1;  //represents % of people voting
map<string, int> Stateresults;    // Map for who won the election
map<string, int> moneycount;      // How much each state costs
map<string, int> turnout;         // How many democratic voters per state
map<string, int> electoral;        // Map that holds states and their  corresponding electoral college votes
mutex Smith;  //Mutex lock for synchronization

int RvsB(int n){ //Function to decide which way the state votes.
    float r; //Random number
    srand(time(0));
    int dem=0;
    int rep=0;
    double rando;
    for(int i=0; i<n; i++){
        r=(double) rand()/(RAND_MAX);
        rando=(double) rand()/(RAND_MAX);
        if ( r < rando){    // if the random number is less than or equal to Chance the voter voted Blue
            dem++;
        }
        else{  //Otherwise the vote goes to the republicans
            rep++;
        }
    }
    return dem;   //Return amount of blue voters.
}

//Function to assign a State to a zone
map<string, int> zoneFinder(string states[]){
    map<string, int> zones;
    string state;
    //If statements, this took a while, matched states to their zones
    for(int i=0; i<50; i++){
        state=states[i];
        if(state=="Washington" || state=="Oregon" || state=="Montana" || state=="California" || state=="Idaho" || state=="Wyoming" || state=="Arizona" || state=="Nevada" || state=="Utah"){
            zones.insert(pair<string, int>(state,6));

        }
        else if(state=="Colorado" || state=="New Mexico" || state=="Texas"|| state=="OKlahoma" || state=="Kansas"|| state=="South Dakota"|| state=="North Dakota" || state=="Nebraska"){
            zones.insert(pair<string, int>(state,5));

        }
        else if(state=="Minnesota" || state=="Iowa" || state=="Wisconsin" || state=="Michigan" || state=="Ohio" || state=="Indiana" || state=="Illinois" || "Missouri"){
            zones.insert(pair<string, int>(state,3));
        }
        else if(state=="Arkansas" || state=="Louisiana" || state=="Mississippi" || state=="Alabama" || state=="Georgia" || state=="Florida"){
            zones.insert(pair<string, int>(state,4));
        }
        else if(state=="South Carolina" || state=="North Carolina" || state=="Tennessee" || state=="Kentucky" || state=="Virginia" || state=="West Virginia" || state=="Maryland" || state=="Pennsylvania"){
            zones.insert(pair<string, int>(state,1));
        }
        else if(state=="New York" || state=="New Jersey" || state=="Delaware" || state=="Connecticut" || state=="Rhode Island" || state=="Massachusetts" || state=="Vermont" || state=="New Hampshire" || state=="Maine"){
            zones.insert(pair<string, int>(state,2));
        }
        else if(state=="Alaska" || state=="Hawaii"){
            zones.insert(pair<string, int>(state,7));
        }
    }
    //Return a reference to the zone map
    return zones;
}

int cost(int zone, int population){
    int rem;
    int cost=0;
    int CVA=3;
    int IntraCost=5; //This cost is fixed, every voter must send the mail to their in state voting collectors, this will cost 5 dollars everytime so it is added to the cost.
    rem=abs(CVA-zone);
    if(rem==0){
        cost=(5+IntraCost)*population; //Each of these are cost * population since everyone in that state will cost the same amount
    }
    else if(rem==1){
        cost=(7+IntraCost)*population;
    }
    else if(rem==2){
        cost=(10+IntraCost)*population;
    }
    else if(rem==3){
        cost=(12+IntraCost)*population;
    }
    else if(rem==4){
        cost=(15+IntraCost)*population;
    }
    else if(rem==5){
        cost=(20+IntraCost)*population;
    }
    else if(rem>=6){
        cost=(40+IntraCost)*population;
    }
    return cost;
}




//Simulate one state
// Get cost of state
// Get result of state


void threadFn(string s, int pop, int zone){
    // 1 represents dems
    // 0 represents republicans
    int dems;
    int chaching;
    string state=s;
    int result;
    int population=pop;
    int realpop=population*voters;
    int mid=realpop/2;
    dems=RvsB(realpop);
    if(dems > mid){   //Democrats were more than half
        result=1;   //Biden wins
    }
    else{   //Democrats were less than half
        result=0;  //Trump wins
    }
    chaching=cost(zone,realpop); // Holds cost for state that this thread was assigned
    Smith.lock(); //Mutex to prevent race conditions
    moneycount.insert(pair<string, int>(state,chaching));  //Map of all the states and the money they used.
    turnout.insert(pair<string, int>(state,dems));   //Insert amount of democratic voters into a map
    Stateresults.insert(pair<string, int>(state,result)); //Insert the states result, Democrat vs Republican
    Smith.unlock(); //Unlock mutex allow another thread to enter.
}

void printstuff(string states[], int populations[]){
    string state;
    string whoWon;
    int numdems, pop, demvote=0, repvote=0, dpopvote=0, reppop=0, runtotal=0, rpopvote=0, final, end,DCpop=495891*voters, DCres;
    double fullcost=0;
    string winner, loser;
    cout << setprecision(20); // set precision so nothing gets cut off
    for(int i=0; i<50; i++){
        state=states[i];
        numdems=turnout[state];
        pop=populations[i]*voters;
        if(Stateresults[state]==1){
            whoWon="Blue";
            demvote+=electoral[state]; // count democrat Electoral College votes
        }
        else{
            repvote+=electoral[state]; // count republican Electoral College votes
            whoWon="Red";
        }
        fullcost+=moneycount[state]; //Add to full cost every iteration
        reppop=pop-numdems; // full population of people who registered to vote, subtract the number of democrats who voted and the result is the republicans
        cout << "The state of " << state << " voted " << whoWon << " with " << numdems << " people voting Blue and " << pop-numdems << " voting Red." << endl;
        cout <<endl;
        runtotal+=pop;
        dpopvote+=numdems;
        rpopvote+=reppop;
    }
    //Special case for DC as it was not in the csv, Got population of registered voters from an external source.
    runtotal+=DCpop;

    DCres=RvsB(DCpop);
    if(DCres > DCpop/2){
        cout << "The state of DC voted Blue with " << DCres << " people voting Blue and " << DCpop-DCres << " Voting Red." << endl;
        dpopvote+=DCres;
        demvote+=3;

    }
    else{
        cout << "The state of DC voted Red with " << DCres << " people voting Blue and " << DCpop-DCres << " Voting Red." << endl;
        rpopvote+=DCpop-DCres;
        repvote+=3;
    }
    cout << endl;

    if(dpopvote>rpopvote){ // If statement to check who won the popular vote
        winner="Democrats";
        final=dpopvote;
        loser="Republicans";
        end=rpopvote;
    }
    else{
        winner="Republicans";
        final=rpopvote;
        loser="Democrats";
        end=dpopvote;

    }
    cout << "*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------*" << endl;
    cout << "The popular vote was won by the " << winner << " who had " << final << " votes" << " while the " << loser << " only had " << end << " out of the total population of " << runtotal << "."<<endl;
    if(repvote>demvote){ // If statement to check who wont the electoral college
        winner="Republicans";
        loser="Democrats";
        final=repvote;
    }
    else{
        loser="Republicans";
        winner="Democrats";
        final=demvote;
    }
    cout << "*--------------------------------------------------------------------------------------------------------------*" << endl;
    cout << "The Electoral College was won by the " << winner << " who won with " <<  final << " votes out of 538." << endl;
    cout << "*--------------------------------------------------------------------------------------------------------------*" << endl;
    cout << "It took 11 days to gather all the mail since the CVA is in Zone 3." << endl;
    cout << "******************************************************************" << endl;
    cout << "The total cost of the election was a whopping $"<< fullcost +cost(1,DCpop)  << " dollars." << endl;
    cout << "******************************************************************" << endl;
}

 void readelec(){
    ifstream po("electoral.csv");
    string statename;
    string elecvote;
    string garb; //used for useless data
    int tpop;
    int count=0;
    if(!po.is_open()){ //Check file
        cout << "Electoral Error" << endl;
    }
    while(po.good()){
        getline(po,statename,',');
        getline(po,elecvote,',');
        getline(po,garb,'\n');
        if(count>0){
            tpop=stoi(elecvote);
            statename.erase(remove( statename.begin(), statename.end(), '\"' ),statename.end()); //stripping quotes.
            electoral.insert(pair<string, int>(statename,tpop)); // add to the electoral map to track their electoral votes
        }
        count++;
    }

}


int main() {
    cout << "Enter a value between 0 and 1, this value will represent the percentage of people who voter per state: ";
    cin >> voters;
    auto start = std::chrono::high_resolution_clock::now(); //Timing
    int *populations;
    map<string, int> zones;
    populations= new int[full]; //Dynamic array for population
    string *states;
    states= new string[full]; //Dynamic array for state names.
    //Specific function for reading the electoral college numbers
    //Felt the need to include this because it would look to cluttered otherwise
    readelec();
    ifstream ip("data.csv");
    if(!ip.is_open()){
        cout << "Error" <<endl;
    }
    int i=0;
    string pop;
    string state;
    string tot;
    string regis;
    string statename;
    int zone;
    int tpop;
    int count=0; //count is only used for first iteration, in order to skip the first line in the csv which includes useless information.
    //Read file for populations and state names
    while(ip.good()){
        //File is read in line by line allowing for parallel arrays, makes coding in the future easier because the data is laid out nicely.
        getline(ip,state,',');
        getline(ip,tot,',');
        getline(ip,regis,',');
        getline(ip,pop,'\n');
        if(count>0){
            //States read in with their corresponding population at the same index.
            tpop=stoi(tot);
            populations[i]=tpop;
            state.erase(remove( state.begin(), state.end(), '\"' ),state.end());
            states[i]=state;
            i++;
        }

        count++;
    }
    //Call zone finder to figure out what states are in what zones
    zones=zoneFinder(states);
    vector<thread> threadz;
    /*
    for(i=0; i<50; i++){
        state=states[i];
        zone=zones[state];
        threadFn(state,populations[i],zone);
    }
*/
    //Send out all 50 threads to simulate 50 mailmen going out and gathering data

   for (i = 0; i < 50; i++){
       state=states[i];
       zone=zones[state];
       threadz.push_back(thread(threadFn,state,populations[i],zone));
   }
   // loop again to join the threads
   for (auto& t : threadz){
       t.join();
   }

   auto stop = std::chrono::high_resolution_clock::now();

   printstuff(states, populations); //print the required output
   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
   cout <<endl << "This program took " << duration.count() << " To execute" << endl;



   return 0;
}