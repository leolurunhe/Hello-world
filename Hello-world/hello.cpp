#include<iostream>
#include<fstream>
#include<sstream>
#include<cstdlib>
#include<string>
#include<cassert>
#include<iomanip>
#include "simulation.h"
#include "world_type.h"
using namespace std;

bool initworld(world_t &world,const string &speciesfile,const string &worldfile){
		unsigned int i=0,j=0;
		ifstream sfile;
		sfile.open(speciesfile.c_str());
		try{
				if(!sfile) throw speciesfile;
		}
		catch(string &wrongfile){
				cout<<"Error: Cannot open file "<<wrongfile<<"!"<<endl;
				return false;
		}
		ifstream wfile;
		wfile.open(worldfile.c_str());
		try{
				if(!wfile) throw worldfile;
		}
		catch(string &file){
				cout<<"Error: Cannot open file "<<file<<"!"<<endl;
				return false;
		}

		string sline;
		getline(sfile,sline);
		unsigned int ns=0;
		while(getline(sfile,sline)){
				ns++;
		}
		try{
				if(ns>MAXSPECIES) throw ns;	
		}
		catch(unsigned int wrongspe){
				cout<<"Error: Too many species!"<<endl;
				cout<<"Maximal number of species is "<<MAXSPECIES<<"."<<endl;
				return false;
		}
		sfile.close();
		world.numSpecies=0;// initialize numSpecies.
		for(i=0;i<MAXSPECIES;i++){// initialize species[MAXSPECIES].
				world.species[i].name="";
				world.species[i].programSize=0;
				for(j=0;j<MAXPROGRAM;j++){
						world.species[i].program[j].op=HOP;
						world.species[i].program[j].address=0;
				}
		}
		world.numCreatures=0;//initialize numCreatures.
		for(i=0;i<MAXCREATURES;i++){//initialize creatures[].
				world.creatures[i].location.r=0;
				world.creatures[i].location.c=0;
				world.creatures[i].direction=EAST;
				world.creatures[i].species=nullptr;
				world.creatures[i].programID=0;
				world.creatures[i].hillActive=false;
				for(j=0;j<ABILITY_SIZE;j++)
						world.creatures[i].ability[j]=false;
		}
		world.grid.height=0;
		world.grid.width=0;
		for(i=0;i<MAXHEIGHT;i++){
				for(j=0;j<MAXWIDTH;j++){
						world.grid.squares[i][j]=nullptr;
				}
		}
		for(i=0;i<MAXHEIGHT;i++){
				for(j=0;j<MAXWIDTH;j++){
						world.grid.terrain[i][j]=PLAIN;
				}
		}
		//above is the initialization of an empty world.
		string wline;
		istringstream istream;
		getline(wfile,wline);
		istream.clear();
		istream.str("");
		istream.str(wline);
		unsigned int row;
		unsigned int column;
		istream>>row;
		try{
				if(row<1 || row>MAXHEIGHT) throw row;
		}
		catch(unsigned int &wrongrow){
				cout<<"Error: The grid height is illegal!"<<endl;
				return false;
		}
		world.grid.height=row;
		getline(wfile,wline);
		istream.clear();
		istream.str("");
		istream.str(wline);
		istream>>column;
		try{
				if(column<1 || column>MAXWIDTH) throw column;
		}
		catch(unsigned int &wrongcolumn){
				cout<<"Error: The grid width is illegal!"<<endl;
				return false;
		}
		world.grid.width=column;
		//above is the height and width of the world.
		try{
				for(i=0;i<row;i++){
						istream.clear();
						istream.str("");
						getline(wfile,wline);
						istream.str(wline);
						for(j=0;j<column;j++){
								if(wline[j]=='P'){
									   				world.grid.terrain[i][j]=PLAIN;	
									   	}
								else if(wline[j]=='L'){
									   				world.grid.terrain[i][j]=LAKE;
									    }
								else if(wline[j]=='F'){
									   				world.grid.terrain[i][j]=FOREST;
								}
								else if(wline[j]=='H'){
									   				world.grid.terrain[i][j]=HILL;
									    }
								else{
													world.grid.terrain[i][j]=TERRAIN_SIZE;
													throw wline[j];
									}
								}
						}						
		}		
		catch(char &wrongline){
				cout<<"Error: Terrain square ("<<wrongline<<" "<<i<<" "<<j<<") is invalid!"<<endl;
				return false;
		}
		wfile.close();
		//above is the terrain part of world.
		wfile.open(worldfile.c_str());
		i=0;
		while(getline(wfile,wline))
				i++;
		unsigned int num=i-2-row;
		try{
				if(num>MAXCREATURES) throw num;
		}
		catch(unsigned int &wrongnum){
				cout<<"Error: Too many creatures!"<<endl;
				cout<<"Maximal number of creatures is "<<MAXCREATURES<<"."<<endl;
				return false;
		}
		world.numCreatures=num;
		wfile.close();
		//above is numCreatures of world.
		string name1,direction1,ability1,ability2,ability;
		ability1="";
		ability2="";
		ability="";
		unsigned int x,y;
		wfile.open(worldfile.c_str());
		getline(wfile,wline);//read row
		getline(wfile,wline);//read column
		string path;
		for(j=0;j<row;j++)
				getline(wfile,wline);//read terrain
		for(i=0;i<num;i++){
				getline(wfile,wline);
				istream.clear();
				istream.str("");
				istream.str(wline);
				istream>>name1>>direction1>>x>>y;
				ability1="";
				ability2="";
				ability="";	
				while(istream.rdbuf()->in_avail()>0){
						string temp="";
						istream>>temp;
						if(temp=="f")
								ability1=temp;
						else if(temp=="a")
								ability2=temp;
						else
								ability=temp;
				}
			
				sfile.open(speciesfile.c_str());
				getline(sfile,sline);
				path=sline;
				int flag=0;
				while(getline(sfile,sline)){
						if(name1==sline){
								flag=1;
								break;
						}
				}
				sfile.close();
				try{if(flag==0) throw flag;}
				catch(int &flag){
						cout<<"Error: species "<<name1<<" not found!"<<endl;
						return false;
				}
					try{
						if(direction1!="east" && direction1!="south" && direction1!="west" && direction1!="north")	throw direction1;
				}
				catch(string &wrongdirection){
						cout<<"Error: Direction "<<wrongdirection<<" is not recognized!"<<endl;
						return false;
				}
				try{
						if(ability!="") throw ability;
				}
				catch(string &wrongability){
						cout<<"Error: Creature ("<<name1<<" "<<direction1<<" "<<x<<" "<<y<<") has an invalid ability "<<wrongability<<"!"<<endl;
						return false;
				}
				if(ability1=="f"){
						world.creatures[i].ability[FLY]=true;
						world.creatures[i].hillActive=true;
				}
				else{
						world.creatures[i].ability[FLY]=false;
						world.creatures[i].hillActive=false;
				}
				if(ability2=="a")
						world.creatures[i].ability[ARCH]=true;
				else
						world.creatures[i].ability[ARCH]=false;
				world.creatures[i].location.r=x;
				world.creatures[i].location.c=y;
				if(direction1=="east")
						world.creatures[i].direction=EAST;
				else if(direction1=="west")
						world.creatures[i].direction=WEST;
				else if(direction1=="south")
						world.creatures[i].direction=SOUTH;
				else if(direction1=="north")
						world.creatures[i].direction=NORTH;
			//	cout<<name1<<endl;
				int flag_1=0;
				if(i==0){
						world.species[0].name=name1;
						world.creatures[0].species=&world.species[0];
				}	
				else{
						for(j=0;j<i;j++){
								if(world.species[j].name==name1){
										world.creatures[i].species=&world.species[j];
										flag_1=1;
										break;
								}
						}
						if(flag_1==0){
								world.species[i].name=name1;
								world.creatures[i].species=&world.species[i];
						}
				}
				ifstream cfile;
				cfile.open(("./"+path+"/"+world.creatures[i].species->name).c_str());
				try{
						if(!cfile) throw world.creatures[i].species->name;
				}
				catch(string &wrongname){
						cout<<"Error: Cannot open file "<<wrongname<<"!"<<endl;
						return false;
				
				}
			
				string line;
				unsigned int cnum;
				unsigned int k=0;
				unsigned int add_ress=0;
				string instruct;
				int flag1=0;
				while(cfile){
						getline(cfile,line);
						istream.clear();
						istream.str("");
						if(line!=""){
								istream.str(line);
								istream>>instruct>>add_ress;
								if(instruct=="hop")
										world.creatures[i].species->program[k].op=HOP;
								else if(instruct=="left")
										world.creatures[i].species->program[k].op=LEFT;
								else if(instruct=="right")
										world.creatures[i].species->program[k].op=RIGHT;
								else if(instruct=="infect")
										world.creatures[i].species->program[k].op=INFECT;
								else if(instruct=="ifempty"){
										world.creatures[i].species->program[k].op=IFEMPTY;
										world.creatures[i].species->program[k].address=add_ress;
								}
								else if(instruct=="ifenemy"){
										world.creatures[i].species->program[k].op=IFENEMY;
										world.creatures[i].species->program[k].address=add_ress;
								}
								else if(instruct=="ifsame"){
										world.creatures[i].species->program[k].op=IFSAME;
										world.creatures[i].species->program[k].address=add_ress;
								}
								else if(instruct=="ifwall"){
										world.creatures[i].species->program[k].op=IFWALL;
										world.creatures[i].species->program[k].address=add_ress;

								}
								else if(instruct=="go"){
										world.creatures[i].species->program[k].op=GO;
										world.creatures[i].species->program[k].address=add_ress;
								}
								else{
										flag1=1;
										break;
								}
								k++;
						}
						else
								break;
				}
				try{
						if(flag1==1) throw flag1;
				}
				catch(int &wrongflag1){
						cout<<"Error: Instruction "<<instruct<<" is not recognized!"<<endl;
						return false;
				}
				try{
						if(k>MAXPROGRAM) throw k;
				}
				catch(unsigned int &wrongpro){
						cout<<"Error: Too many instructions for species "<<world.creatures[i].species->name<<"!"<<endl;
						cout<<"Maximal number of instructions is "<<MAXPROGRAM<<"."<<endl;
						return false;
				}
				world.creatures[i].species->programSize=k;
				world.creatures[i].programID=0;
				
				try{
						if(x>=world.grid.height){
								throw x;
						}
						else if(y>=world.grid.width)
								throw y;
				}
				catch(unsigned int &boundary){
						cout<<"Error: Creature ("<<name1<<" "<<direction1<<" "<<x<<" "<<y<<") is out of bound!"<<endl;
						cout<<"The grid size is "<<world.grid.height<<" by "<<world.grid.width<<"."<<endl;
						return false;
				}
		
				try{	
						if(world.grid.squares[x][y]==nullptr)
								world.grid.squares[x][y]=&world.creatures[i];
						else
								throw world.grid.squares[x][y];
				}
				catch(creature_t *wrongsquares){
						cout<<"Error: Creature ("<<name1<<" "<<direction1<<" "<<x<<" "<<y<<") overlaps with creature ("<<wrongsquares->species->name<<" "<<directName[wrongsquares->direction]<<" "<<x<<" "<<y<<")!"<<endl;
						return false;
				}
						
				int wronglake=0;
				try{
						if(world.grid.terrain[x][y]==LAKE && !world.creatures[i].ability[FLY])
								throw wronglake;
				}
				catch(int &wrong_lake){
						cout<<"Error: Creature ("<<name1<<" "<<direction1<<" "<<x<<" "<<y<<") is in a lake square!"<<endl;
						cout<<"The creature cannot fly!"<<endl;
						return 0;
				}
				sfile.close();
				cfile.close();
		}
		unsigned int num_species=0;
		for(i=0;i<MAXSPECIES;i++){
				if(world.species[i].name!="")
						num_species++;
		}
		world.numSpecies=num_species;
		return true;
}


void printGrid(const grid_t grid){
		unsigned int i,j;
		for(i=0;i<grid.height;i++){
				for(j=0;j<grid.width;j++){
						if(grid.squares[i][j]==nullptr)
								cout<<"____ ";
						else
								cout<<grid.squares[i][j]->species->name[0]<<grid.squares[i][j]->species->name[1]<<"_"<<directShortName[grid.squares[i][j]->direction]<<" ";
				}
		cout<<endl;
		}
}

point_t ajacentPoint(point_t pt,direction_t dir){
		if(dir==EAST)
				pt.c++;
		if(dir==WEST)
				pt.c--;
		if(dir==NORTH)
				pt.r--;
		if(dir==SOUTH)
				pt.r++;
		return pt;
}

direction_t leftFrom(direction_t dir){
		if(dir==EAST)
				dir=NORTH;
		else if(dir==NORTH)
				dir=WEST;
		else if(dir==WEST)
				dir=SOUTH;
		else if(dir==SOUTH)
				dir=EAST;
		return dir;
}

direction_t rightFrom(direction_t dir){
		if(dir==EAST)
				dir=SOUTH;
		else if(dir==SOUTH)
				dir=WEST;
		else if(dir==WEST)
				dir=NORTH;
		else if(dir==NORTH)
				dir=EAST;
		return dir;
}

instruction_t getInstruction(const creature_t &creature){
		return creature.species->program[creature.programID];
}

creature_t* getCreature(const grid_t &grid,point_t location){
		return grid.squares[location.r][location.c];
}

bool ifWall(world_t &world,creature_t* creature){
		point_t pt=ajacentPoint(creature->location,creature->direction);
		return (pt.c<0 || pt.r<0 || pt.r>=world.grid.height || pt.c>=world.grid.width || (!creature->ability[FLY] && world.grid.terrain[pt.r][pt.c]==LAKE));
}

void if_wall(world_t &world,creature_t* creature,unsigned int address){
		if(ifWall(world,creature))
				creature->programID=address-1;
		else
				creature->programID++;
}

bool ifSame(world_t &world,creature_t* creature){
		point_t pt=ajacentPoint(creature->location,creature->direction);
		return (!ifWall(world,creature)) && (getCreature(world.grid,pt)!=nullptr) && (getCreature(world.grid,pt)->species==creature->species) && (world.grid.terrain[pt.r][pt.c]!=FOREST || creature->ability[ARCH]);
}

void if_same(world_t &world,creature_t* creature,unsigned int address){
		if(ifSame(world,creature))
				creature->programID=address-1;
		else
				creature->programID++;
}

bool ifEmpty(world_t &world,creature_t* creature){
		point_t pt=ajacentPoint(creature->location,creature->direction);
		if(!creature->ability[ARCH]){
				if(world.grid.terrain[pt.r][pt.c]==FOREST)
						return false;
				else if(pt.c<0)
						return false;
				else if(pt.r<0)
						return false;
				else if(pt.r>=world.grid.height)
						return false;
				else if(pt.c>=world.grid.width)
						return false;
				else{
						if(getCreature(world.grid,pt)==nullptr)
								return true;
						if(getCreature(world.grid,pt)!=nullptr)
								return false;
				}
		}
		else{	if(pt.c<0)
						return false;
				else if(pt.r<0)
						return false;
				else if(pt.r>=world.grid.height)
						return false;
				else if(pt.c>=world.grid.width)
						return false;
				else{
						if(getCreature(world.grid,pt)==nullptr)
								return true;
						if(getCreature(world.grid,pt)!=nullptr)
								return false;
				}
		}
}

void if_empty(world_t &world,creature_t* creature,unsigned int address){
		if(ifEmpty(world,creature))
				creature->programID=address-1;
		else
				creature->programID++;
}

void go(world_t &world,creature_t* creature,unsigned int address){
		creature->programID=address-1;
}

bool ifEnemy(world_t &world,creature_t* creature){
		point_t pt=ajacentPoint(creature->location,creature->direction);
		if(creature->ability[ARCH]){
				if(ifEmpty(world,creature))
						return false;
				else if(ifWall(world,creature))
						return false;
				else if(ifSame(world,creature))
						return false;
				else
						return true;
		}
		else{
				if(ifEmpty(world,creature))
						return false;
				else if(ifWall(world,creature))
						return false;
				else if(world.grid.terrain[pt.r][pt.c]==FOREST)
						return false;
				else if(ifSame(world,creature))
						return false;
				else
						return true;
		}
}

void if_enemy(world_t &world,creature_t* creature,unsigned int address){
		if(ifEnemy(world,creature))
				creature->programID=address-1;
		else
				creature->programID++;
}

void left(creature_t* creature){
		creature->direction=leftFrom(creature->direction);
		creature->programID++;
}

void right(creature_t* creature){
		creature->direction=rightFrom(creature->direction);
		creature->programID++;
}

void hop(world_t &world,creature_t* creature){
		point_t pt=ajacentPoint(creature->location,creature->direction);
		if(pt.r>=0 && pt.c>=0 && pt.r<world.grid.height && pt.c<world.grid.width && (world.grid.terrain[pt.r][pt.c]!=LAKE || creature->ability[FLY]) && getCreature(world.grid,pt)==nullptr){
				world.grid.squares[creature->location.r][creature->location.c]=nullptr;
				world.grid.squares[pt.r][pt.c]=creature;
				creature->location=pt;
		}
		creature->programID++;
}


void infect(world_t &world,creature_t* creature){
		point_t pt=ajacentPoint(creature->location,creature->direction);
		if(!creature->ability[ARCH]){
				if(pt.r>=0 && pt.c>=0 && pt.r<world.grid.height && pt.c<world.grid.width && getCreature(world.grid,pt)!=nullptr && getCreature(world.grid,pt)->species!=creature->species && world.grid.terrain[pt.r][pt.c]!=FOREST){
						getCreature(world.grid,pt)->species=creature->species;
						getCreature(world.grid,pt)->programID=0;
				}
		}
		else if(creature->ability[ARCH]){
				while(getCreature(world.grid,pt)==nullptr || getCreature(world.grid,pt)->species==creature->species){
						pt=ajacentPoint(pt,creature->direction);
					//	cout<<pt.r<<endl;
					//	cout<<pt.c<<endl;
						if(pt.r<0 || pt.c<0 || pt.r>=world.grid.height || pt.c>=world.grid.width)
								break;
				}
				if(pt.r>=0 && pt.c>=0 && pt.r<world.grid.height && pt.c<world.grid.width && getCreature(world.grid,pt)!=nullptr && getCreature(world.grid,pt)->species!=creature->species){
						getCreature(world.grid,pt)->species=creature->species;
						getCreature(world.grid,pt)->programID=0;
				}
		}
		creature->programID++;
}

void simulateCreature(creature_t &creature,world_t &world,bool verbose){
		if(verbose){
				cout<<"Creature ("<<creature.species->name<<" "<<directName[creature.direction]<<" "<<creature.location.r<<" "<<creature.location.c<<") takes action:"<<endl;
				instruction_t order=getInstruction(creature);
				if(order.address!=0)
						cout<<"Instruction "<<creature.programID+1<<": "<<opName[order.op]<<" "<<order.address<<endl;
				else
						cout<<"Instruction "<<creature.programID+1<<": "<<opName[order.op]<<endl;
				if(order.op==HOP)
						hop(world,&creature);
				else if(order.op==LEFT)
						left(&creature);
				else if(order.op==RIGHT)
						right(&creature);
				else if(order.op==INFECT)
						infect(world,&creature);
				else if(order.op==IFEMPTY)
						if_empty(world,&creature,order.address);
				else if(order.op==IFENEMY)
						if_enemy(world,&creature,order.address);
				else if(order.op==IFSAME)
						if_same(world,&creature,order.address);
				else if(order.op==IFWALL)
						if_wall(world,&creature,order.address);
				else if(order.op==GO)		
						go(world,&creature,order.address);
				while(order.op==IFEMPTY || order.op==IFSAME || order.op==IFWALL || order.op==GO || order.op==IFENEMY){
						order=getInstruction(creature);
						if(order.address!=0)
								cout<<"Instruction "<<creature.programID+1<<": "<<opName[order.op]<<" "<<order.address<<endl;
						else
								cout<<"Instruction "<<creature.programID+1<<": "<<opName[order.op]<<endl;
						if(order.op==HOP)
								hop(world,&creature);
						else if(order.op==LEFT)
								left(&creature);
						else if(order.op==RIGHT)
								right(&creature);
						else if(order.op==INFECT)
								infect(world,&creature);
						else if(order.op==IFEMPTY)
								if_empty(world,&creature,order.address);
						else if(order.op==IFENEMY)
								if_enemy(world,&creature,order.address);
						else if(order.op==IFSAME)
								if_same(world,&creature,order.address);
						else if(order.op==IFWALL)
								if_wall(world,&creature,order.address);
						else if(order.op==GO)
								go(world,&creature,order.address);
						
						}
				printGrid(world.grid);
		}
		else{
				instruction_t shortorder=getInstruction(creature);
				cout<<"Creature ("<<creature.species->name<<" "<<directName[creature.direction]<<" "<<creature.location.r<<" "<<creature.location.c<<") takes action: ";
				if(shortorder.op==HOP)
						hop(world,&creature);
				else if(shortorder.op==LEFT)
						left(&creature);
				else if(shortorder.op==RIGHT)
						right(&creature);
				else if(shortorder.op==INFECT)
						infect(world,&creature);
				else if(shortorder.op==IFEMPTY)
						if_empty(world,&creature,shortorder.address);
				else if(shortorder.op==IFENEMY)
						if_enemy(world,&creature,shortorder.address);
				else if(shortorder.op==IFSAME)
						if_same(world,&creature,shortorder.address);
				else if(shortorder.op==IFWALL)
						if_wall(world,&creature,shortorder.address);
				else if(shortorder.op==GO)		
						go(world,&creature,shortorder.address);
				while(shortorder.op==IFEMPTY || shortorder.op==IFSAME || shortorder.op==IFWALL || shortorder.op==GO || shortorder.op==IFENEMY){
						shortorder=getInstruction(creature);
						if(shortorder.op==HOP)
								hop(world,&creature);
						else if(shortorder.op==LEFT)
								left(&creature);
						else if(shortorder.op==RIGHT)
								right(&creature);
						else if(shortorder.op==INFECT)
								infect(world,&creature);
						else if(shortorder.op==IFEMPTY)
								if_empty(world,&creature,shortorder.address);
						else if(shortorder.op==IFENEMY)
								if_enemy(world,&creature,shortorder.address);
						else if(shortorder.op==IFSAME)
								if_same(world,&creature,shortorder.address);
						else if(shortorder.op==IFWALL)
								if_wall(world,&creature,shortorder.address);
						else if(shortorder.op==GO)		
								go(world,&creature,shortorder.address);
				}
				cout<<opName[shortorder.op]<<endl;
		}				
}
