//
// Author  : <Güneykan Özgül> 
//           Bogazici University, Computer Engineering
// Project ://Implementation Of Discrete Event Simulation in C++.
//The code below is only for illustration purposes and neither well optimised nor is the best way of implementing any method.

#include <iostream>
#include <queue>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <math.h>
#include <iomanip>


// Compile command:  g++ --std=c++11 main.cc -o dse
// Run command 1:    ./dse filename

using namespace std;
struct tup{
    int index;
    double time;
    bool operator<( tup other) const
    {
        return index >= other.index;
    }

};
//Represents a unit in the system.
struct Unit{
    //maximum length of queue of this unit.
    int max_length;
    //busy time of this unit during the whole simulation.
    double busy_time;
    //Time in which this unit can process one job.
    double run_time;
    //Next time that this unit is available.
    double free;
    int q_length;
    Unit(){
        free=0;
        busy_time=0;
        max_length=0;
        q_length=0;
    }
    //Process a job and calculate its process time in this unit.
    double process(double time){

        double e_time;
        if(time<free){
            e_time = free + run_time;
            max_length=max(max_length,(int)((e_time-time)/run_time));
            q_length=(int)((e_time-time)/run_time);
    }
        else{

            e_time = time + run_time;
        }
        busy_time+=run_time;
        free=e_time;

        return e_time;

    }
};
//Represents an event.
struct Event{
    int index;
    int to;
    int from;
    double time;
    bool operator<( Event other) const
    {
        return time >= other.time;
    }
};
//Simulates sequence of events in units.
struct Simulation{
    //Creates a graph structure that connects units using given parameters.
    //Simulates events considering running time of units an entrance time of the events to the system.
    //Generates random numbers to determine flow of a job in the system.
    //Calculates and prints some statistical data about the system.(Utilizations of units, Turn Around Times, Total time).
    void simulate(int type,string input,string out) {
        ifstream my_file(input);
        vector<struct Unit> units;
        int seed=0;
        signed int number_of_units = 0;
        int x;
        int y;
        vector<vector<int>> adjlist;
        //Create graph structure and units.
        if (my_file.is_open()) {
            std::srand(std::time(0));
            my_file  >> number_of_units;
            string consume = " ";
            adjlist.resize(number_of_units);
            getline(my_file, consume);
            string temp;
            double time = 0;
            for (int i = 0; i < number_of_units; i++) {
                Unit *unit = new Unit();
                getline(my_file, temp);
                stringstream ss(temp);
                ss >> x;
                ss >> time;
                unit->run_time = time;
                units.push_back(*unit);
                if (!ss.eof()) {
                    while (ss >> y) {
                        adjlist[x].push_back(y);
                    }
                }

            }


        } else {cout << "Where is the input file?";}
        priority_queue<struct Event> event_list;
        vector<double> arrivals;
        int number_of_events;
        double time = 0;
        my_file >> number_of_events;
        priority_queue<tup> end_time;
        double total = 0;
        //Create events.
        for (int i = 0; i < number_of_events; i++) {
            Event event;
            event.to = 0;
            event.from=-1;
            event.index = i;
            my_file >> event.time;
            arrivals.push_back(event.time);
            event_list.push(event);
        }
        //Iterates until all events are done.
        while (!event_list.empty()) {
            Event e = event_list.top();
            event_list.pop();
            time = e.time;
            int indeg=1;
            if(e.from!=-1)
                indeg = adjlist[e.from].size();
            Event f;

            double shortest_q= INFINITY;
            int s_index=0;
            if (indeg != 0) {
                if(e.from!=-1){
                     //e.to=adjlist[e.from][floor((double) rand() * indeg / RAND_MAX)];
                        if(units[e.from].q_length!=0)
                        units[e.from].q_length--;
                    for (int i = 0; i < indeg; i++) {
                        if(units[adjlist[e.from][i]].q_length<shortest_q ){
                          shortest_q= units[adjlist[e.from][i]].q_length;

                            s_index=i;

                        }

                    }
                    if(type==1){
                   e.to = adjlist[e.from][s_index];}
                    else if (type==0){
                     e.to=adjlist[e.from][floor((double) rand() * indeg / RAND_MAX)];}
               }else{                     

                }
                f.from=e.to;
                f.index = e.index;
                f.time = units[e.to].process(time);
                event_list.push(f);
            } else {
                if(units[e.from].q_length!=0)
                    units[e.from].q_length--;
                tup t;
                t.index = e.index;
                t.time = e.time - arrivals[e.index];
                end_time.push(t);
                if(e.time>total)
                    total=e.time;
            }

        }
        //Output statistical data.
        ofstream output(out);
        output <<  std::fixed << std::setprecision(4) <<total << endl;
        for (int i = 0; i < units.size(); i++) {
            output << i << " ";
            double val = (units[i].busy_time / total);
            double nearest = roundf(val * 100) / 100;
            output <<std::fixed << std::setprecision(2)<< nearest << " " << units[i].max_length << endl;
        }
        for (int i = 0; i < number_of_events ; i++) {
            output <<std::fixed << std::setprecision(4)<< i << " " << end_time.top().time << endl;
            end_time.pop();

        }
    }
};
int main(int argc, char*argv[]){
    string in=argv[1];
    string out=argv[2];
    string out2=argv[3];
    Simulation s;
    s.simulate(1,in,out);
    s.simulate(0,in,out2);


    return 0;
}
