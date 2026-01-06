#include<iostream>
#include<iomanip>
#include<cstdlib>
#include<fstream>
#include<vector>
#include<algorithm>
#include<ctime>
#include<tuple>
#include<limits>
#include<map>
using namespace std;
string hashPassword(const string&p){
    unsigned long h=5381;
    for(char c:p)
        h=((h<<5)+h)+c;
    return to_string(h);
}
bool passwordMatches(const string& input, const string& storedHash){
    return hashPassword(input)==storedHash;
}
string currentDate() {
    time_t t=time(nullptr);
    tm now{};
    localtime_s(&now,&t);
    char buf[11];
    strftime(buf,sizeof(buf),"%Y-%m-%d",&now);
    return buf;
}
string currentTime(){
    time_t t=time(nullptr);
    tm now{};
    localtime_s(&now,&t);
    char buf[9];
    strftime(buf,sizeof(buf),"%H:%M:%S",&now);
    return buf;
}
bool readInt(int& v){
    if(!(cin>>v)){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        return false;
    }
    return true;
}
bool readDouble(double& v){
    if(!(cin>>v)){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        return false;
    }
    return true;
}
bool readIntCancelable(int& v){
    if(!readInt(v)) 
		return false;
    return v!=0;
}
bool readDoubleCancelable(double& v){
    if(!readDouble(v)) 
		return false;
    return v!=0.0;
}
struct Food{
    int id;
    string name;
    string type;
    double price;
};
struct Reservation{
    int id;
    unsigned long long studentID;
    int foodID;
    string date;
    string time;
    string qr;
};
struct Student{
    unsigned long long id;
    string username;
    string passHash;
    double wallet;
};
struct Admin {
    string username;
    string passHash;
};
vector<Student>students;
vector<Food>foods;
vector<Reservation>reservations;
vector<Admin>admins;
double totalIncome=0.0;
double totalRefund=0.0;
int nextFoodID=1;
int nextReservationID=1;
void ClearScreen(){
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
void saveAll(){
    ofstream s("students.txt");
    for(auto& st:students)
        s<<st.id<<" "<<st.username<<" "<<st.passHash<<" "<<st.wallet<<"\n";
    ofstream f("foods.txt");
    for(auto& fd:foods)
        f<<fd.id<<" "<<fd.name<<" "<<fd.type<<" "<<fd.price<<"\n";
    ofstream r("reservations.txt");
    for(auto& rs:reservations)
        r<<rs.id<<" "<<rs.studentID<<" "<<rs.foodID<<" "<<rs.date<<" "<<rs.time<<" "<<rs.qr<<"\n";
    ofstream rep("reports.txt");
    rep<<totalIncome<<"\n"<<totalRefund<<"\n";
    ofstream a("admins.txt");
    for(auto& ad:admins)
        a<<ad.username<<" "<<ad.passHash<<"\n";
}
void loadAll(){
    students.clear();
    foods.clear();
    reservations.clear();
    admins.clear();
    totalIncome=totalRefund=0;
    nextFoodID=nextReservationID=1;
    Student s;
    ifstream sf("students.txt");
    while(sf>>s.id>>s.username>>s.passHash>>s.wallet){
        students.push_back(s);
    }
    Food f;
    ifstream ff("foods.txt");
    while(ff>>f.id>>f.name>>f.type>>f.price){
        foods.push_back(f);
        nextFoodID=max(nextFoodID,f.id+1);
    }
    Reservation r;
    ifstream rf("reservations.txt");
    while(rf>>r.id>>r.studentID>>r.foodID>>r.date>>r.time>>r.qr){
        reservations.push_back(r);
        nextReservationID=max(nextReservationID,r.id+1);
    }
    ifstream rep("reports.txt");
    if(rep)
        rep>>totalIncome>>totalRefund;
    Admin a;
    ifstream af("admins.txt");
    while(af>>a.username>>a.passHash)
        admins.push_back(a);

    if(admins.empty())
        admins.push_back({"admin", hashPassword("admin")});
}
Food*getFoodByID(int id){
    for(auto& f:foods)
        if(f.id==id)
            return &f;
    return nullptr;
}
Student*getStudentByID(unsigned long long id){
    for(auto& s:students)
        if(s.id==id)
            return &s;
    return nullptr;
}
bool alreadyReservedToday(unsigned long long sid){
    for(auto& r:reservations)
        if(r.studentID==sid && r.date==currentDate())
            return true;
    return false;
}
string generateQR(){
    const char charset[]=
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const int length=3;
    string qr="QR#";
    for(int i=0;i<length;i++){
        qr+=charset[rand()%(sizeof(charset)-1)];
    }
    return qr;
}
void listFoods(){
    if(foods.empty()){
        cout<<"No food available.\n";
        return;
    }
    cout<<"\n\t---------- Food List ----------\n";
    cout<<left<<setw(5)<<"ID"<<setw(16)<<"Name"<<setw(12)<<"Type"<<setw(6)<<"Price\n"; 
    cout<<"-----------------------------------------------------\n"; 
    for(const auto& f:foods){
        cout<<left<<setw(5)<<f.id<<setw(16)<<f.name<<setw(10)
		<<f.type<<right<<setw(10)<<fixed<<setprecision(2)<<f.price<<"$\n"; 
    }
    cout<<"-----------------------------------------------------\n";
}
void listStudentReservations(const Student& st){
    bool hasReservations=false;
    cout<<"\n---------------- My Reservations ----------------\n";
    cout<<left<<setw(16)<<"Food"<<setw(12)<<"Type"<<setw(10)<<"Price"<<setw(12)<<"Date"<<setw(10)<<"Time"<<"QR\n";
    cout<<"-----------------------------------------------------------------\n";
    for(const auto& r:reservations){
        if(r.studentID!=st.id)
			continue;
        hasReservations=true;
        Food*f=getFoodByID(r.foodID);
        cout<<left<<setw(16)<<(f?f->name:"N/A")<<setw(12)<<(f?f->type:"N/A")<<setw(10)
		<<fixed<<setprecision(2)<<(f?f->price:0.0)<<setw(12)<<r.date<<setw(10)<<r.time<<r.qr<<"\n";
    }
    if(!hasReservations)
        cout<<"No reservations found.\n";
    cout<<"-----------------------------------------------------------------\n";
}
void adminMenu(Admin& currentAdmin){
    while(true){
        cout<<"\n===== Admin Menu =====\n";
        cout<<"1. Change your username/password\n";
        cout<<"2. Add Student\n";
        cout<<"3. Delete Student\n";
		cout<<"4. Edit student info\n";
		cout<<"5. List of Students\n";
        cout<<"6. Add Food\n";
        cout<<"7. Delete Food\n";
        cout<<"8. Edit Food info\n";
        cout<<"9. List of Foods\n";
        cout<<"10.View Reservations\n";
        cout<<"11.Reports\n";
    	cout<<"0. Logout\n";
        int choice;
        if(!readInt(choice)) 
			continue;
        if(choice==0){
			ClearScreen(); 
			return;
		}
		if(choice==1){
    		cout<<"1. Change Username\n";
    		cout<<"2. Change Password\n";
    		cout<<"0. Cancel\n";
    		int opt;
    		if(!readInt(opt)||opt==0) 
				continue;
    		if(opt==1){
        		cout<<"Enter new username(0 to cancel): ";
        		string newUser;
        		cin>>newUser;
        		bool exists=false;
        		for(const auto& a:admins){
            		if(a.username==newUser){
                		exists=true;
                		break;
            		}
        		}
        		if(exists){
            		cout<<"Username already exists.\n";
            		continue;
        		}
        		currentAdmin.username=newUser;
        		saveAll();
        		cout<<"Username updated successfully.\n";
    		}
    		else if(opt==2){
        		cout<<"Enter new password(0 to cancel): ";
        		string newPass;
        		cin>>newPass;
        		currentAdmin.passHash=hashPassword(newPass);
        		saveAll();
        		cout<<"Password updated successfully.\n";
    		}
		}
        else if(choice==2){
    		Student s;
    		cout<<"Enter 12-digit Student ID(0 to cancel): ";
    		unsigned long long idInput;
    		if(!(cin>>idInput)){
        		cin.clear();
        		cin.ignore(numeric_limits<streamsize>::max(),'\n');
        		cout<<"Invalid input.\n";
        		continue;
    		}
    		if(idInput<100000000000ULL||idInput>999999999999ULL){
        		cout<<"Student ID must be exactly 12 digits.\n";
        		continue;
    		}
    		bool exists=false;
    		for(auto& st:students){
        		if(st.id==idInput){
            		exists=true;
            		break;
        		}
    		}
    		if(exists){
        		cout<<"Student ID already exists.\n";
        		continue;
    		}
    		s.id=idInput;
			cin.ignore();
    		cout<<"Username: ";
    		getline(cin,s.username);
    		exists=false;
    		for(auto& st:students){
        		if(st.username==s.username){
            		exists=true;
            		break;
        		}
    		}
    		if(exists){
        		cout<<"Username already exists.\n";
        		continue;
    		}
    		cout<<"Password: ";
    		string p;
    		getline(cin,p);
    		s.passHash=hashPassword(p);
    		s.wallet=0;
    		students.push_back(s);
    		saveAll();
    		cout<<"A new student has been added successfully.\n";
		}
        else if(choice==3){
            cout<<"Student ID(0 to cancel): ";
            unsigned long long id;
			if(!(cin>>id)){
    			cin.clear();
    			cin.ignore(numeric_limits<streamsize>::max(),'\n');
    			cout<<"Invalid input.\n";
    			continue;
			}
			bool studentExists=false;
            for(auto it=reservations.begin();it!=reservations.end();){
                if(it->studentID==id){
                    Food*f=getFoodByID(it->foodID);
                    if(f){
                        totalIncome-=f->price;
                        totalRefund+=f->price;
                    }
                    it=reservations.erase(it);
                } 
				else 
					++it;
            }
            auto it=remove_if(students.begin(),students.end(),[&](const Student& s){return s.id==id;});
    		if(it!=students.end()){
        		studentExists=true;
        		students.erase(it,students.end());
        		saveAll();
        		cout<<"Student has been deleted successfully.\n"; 
    		}
    		if(!studentExists)
        		cout<<"Student ID not found.\n";
        }
		else if(choice==4){
    		cout<<"Enter the current 12-digit Student ID to edit(0 to cancel): ";
    		unsigned long long id;
    		if(!(cin>>id)){
        		cin.clear();
        		cin.ignore(numeric_limits<streamsize>::max(),'\n');
        		cout<<"Invalid input.\n";
        		continue;
    		}
    		Student*stPtr=nullptr;
    		for(auto& s:students){
        		if(s.id==id){
            		stPtr=&s;
            		break;
        		}
    		}
    		if(!stPtr){
        		cout<<"Student ID not found.\n";
        		continue;
    		}
    		cout<<"\nEditing Student: "<<stPtr->username<<" ("<<stPtr->id<<")\n";
    		cout<<"Choose what to edit:\n";
    		cout<<"1. Change Name\n";
    		cout<<"2. Change Wallet Balance\n";
    		cout<<"3. Change Student ID\n";
    		cout<<"0. Cancel\n";
    		int editChoice;
    		if(!readInt(editChoice))
        		continue;
    		if(editChoice==0)
				continue;
    		if(editChoice==1){
        		cout<<"Enter new username: ";
        		string newName;
        		cin >> newName;
        		bool exists = false;
        		for(auto& s:students){
            		if(s.username==newName && &s!=stPtr){
                		exists=true;
                		break;
            		}
        		}
        		if(exists){
            		cout<<"Username already exists.\n";
            		continue;
        		}
        		stPtr->username=newName;
        		cout<<"Username updated successfully.\n";
    		}
    		else if(editChoice==2){
        		cout<<"Enter new wallet balance: ";
        		double newWallet;
        		if(!readDouble(newWallet)||newWallet<0){
            		cout<<"Invalid amount.\n";
            		continue;
        		}
        		stPtr->wallet=newWallet;
        		cout<<"Wallet updated successfully.\n";
    		}
    		else if(editChoice==3){
        		cout<<"Enter new 12-digit Student ID: ";
        		unsigned long long newID;
        		if(!(cin>>newID)||newID<100000000000ULL||newID>999999999999ULL){
            		cout<<"Invalid Student ID.\n";
            		continue;
        		}
        		bool exists=false;
        		for(auto& s:students){
            		if(s.id==newID && &s!=stPtr){
                		exists=true;
                		break;
            		}
        		}
        		if(exists){
            		cout<<"Student ID already exists.\n";
            		continue;
        		}
        		for(auto& r:reservations){
            		if(r.studentID==stPtr->id){
                		r.studentID=newID;
            		}
        		}
        		stPtr->id=newID;
        		cout<<"Student ID updated successfully.\n";
    		}
    		saveAll();
		}
		else if(choice==5){  
        	if(students.empty()){
            cout<<"No students available.\n";
        	}
			else{
				sort(students.begin(),students.end(),[](const Student &a,const Student &b){
            	return a.id<b.id;
        		});
            	cout<<"\n      ------- List of Students -------\n";
            	cout<<left<<setw(15)<<"ID"<<setw(20)<<"Name"<<setw(5)<<"Wallet\n";
            	for(auto& st:students){
					cout<<left<<setw(15)<<st.id<<setw(20)<<st.username
					<<setw(9)<<fixed<<setprecision(2)<<st.wallet<<"$\n";
            	}
            	cout << "----------------------------------------------\n";
    		}
		}
        else if(choice==6){
            Food f;
			f.id=nextFoodID++;
			cout<<"Enter Food Name(0 to cancel): ";
			cin>>f.name;
			if(f.name=="0"){
    			continue;
			}
			cout<<"Enter Food Type(0 to cancel): ";
			cin>>f.type;
			if(f.type=="0"){
    			continue;
			}
			cout<<"Enter Price(0 to cancel): ";
			if(!readDouble(f.price)||f.price==0){
    			continue;
			}
			foods.push_back(f);
			saveAll();
			cout<<"Food has been added successfully.\n";
		}
        else if(choice==7){
            cout<<"Food ID(0 to cancel): ";
            int id;
            if(!readInt(id)) 
				continue;
				Food*fptr=getFoodByID(id);
				if(!fptr){
    			cout<<"Food not found.\n";
    			continue;
			}
            bool inUse=false;
            for(auto& r:reservations)
                if(r.foodID==id)
                    inUse=true;
            if(inUse){
                cout<<"Food has active reservations.\n";
                continue;
            }
            foods.erase(remove_if(foods.begin(),foods.end(),[&](const Food& f){return f.id==id;}),
                foods.end());
            saveAll();
            cout<<"Food has been deleted successfully.\n"; 
        }
        else if(choice==8){
    		listFoods();  
    		cout<<"Enter Food ID to edit(0 to cancel): ";
    		int foodId;
    		if(!readInt(foodId)) 
			continue;
    		Food*foodPtr=getFoodByID(foodId);
    		if(!foodPtr){
        		cout<<"Food not found.\n";
        		continue;
    		}
    		cout<<"Editing food: "<<foodPtr->name<<" (ID: "<<foodPtr->id<<")\n";
    		cout<<"1. Change Name\n";
    		cout<<"2. Change Type\n";
    		cout<<"3. Change Price\n";
    		cout<<"0. Cancel\n";
    		int editChoice;
    		if(!readInt(editChoice)) 
				continue;
    		if(editChoice==1){
        		cout<<"Enter new name: ";
        		cin>>foodPtr->name;
        		cout<<"Name updated successfully.\n";
    		} 
    		else if(editChoice==2){
        		cout<<"Enter new type: ";
        		cin>>foodPtr->type;
        		cout<<"Type updated successfully.\n";
    		} 
    		else if(editChoice==3){
        		cout<<"Enter new price: ";
        		double newPrice;
        		if(readDouble(newPrice) && newPrice>0){
            		foodPtr->price=newPrice;
            		cout<<"Price updated successfully.\n";
        		} 
				else{
            		cout<<"Invalid price.\n";
        		}
    		} 
    		else if(editChoice==0){
        		continue; 
    		}
    		saveAll();
		}
        else if(choice==9){
    		listFoods();
		}
        else if(choice==10){
    		if(reservations.empty()){
        		cout<<"No reservations found.\n";
        		continue;
    		}
    		sort(reservations.begin(),reservations.end(),[](const Reservation& a,const Reservation& b){
            	return tie(a.date,a.time)<tie(b.date,b.time);
        	});
    		cout<<"\n\t\t-------------------- Reservations --------------------\n";
    		cout<<left<<setw(14)<<"Student ID"<<setw(20)<<"Username"<<setw(16)<<"Food"
        		<<setw(10)<<"Type"<<setw(9)<<"Price"<<setw(12)<<"Date"<<setw(10)<<"Time"<<"QR\n";
    		cout<<"------------------------------------------------------------------------------------------------\n";
    		for(const auto& r:reservations){
        		Food*f=getFoodByID(r.foodID);
        		Student* s=getStudentByID(r.studentID);
        		cout<<left<<setw(14)<<r.studentID<<setw(20)<<(s?s->username:"N/A")
				<<setw(16)<<(f?f->name:"N/A")<<setw(10)<<(f?f->type:"N/A")<<setw(9)<<fixed<<setprecision(2)
				<<(f?f->price:0.0)<<setw(12)<<r.date<<setw(10)<<r.time<<r.qr<<"\n";
    		}
    		cout<<"------------------------------------------------------------------------------------------------\n";
		}
        else if(choice==11){
    		cout<<"\n---- Reports ----\n";
    		cout<<"Total Income: "<<fixed<<setprecision(2)<<totalIncome<<"$\n";
    		cout<<"Total Refunds: "<<fixed<<setprecision(2)<<totalRefund<<"$\n";
    		if(!foods.empty() && !reservations.empty()){
        		map<int,int>foodCount; 
        		for(auto& r:reservations){
            		foodCount[r.foodID]++;
        		}
        		int maxCount=0;
        		for(auto& [fid,cnt]:foodCount){
            		if(cnt>maxCount)maxCount=cnt;
        			}
        		cout<<"Most Reserved Food(s):\n";
        		for(auto& [fid,cnt]:foodCount){
            		if(cnt==maxCount){
                		Food*f=getFoodByID(fid);
                		if(f)
                    		cout<<" - "<<f->name <<"("<<cnt<<" reservations)\n";
            		}
        		}
        		map<unsigned long long,int>studentCount; 
        		for(auto& r:reservations){
            		studentCount[r.studentID]++;
        		}
        		int maxStudentCount=0;
        		for(auto& [sid,cnt]:studentCount){
            		if(cnt>maxStudentCount) 
						maxStudentCount=cnt;
        			}
        		cout<<"Students with Most Reservations:\n";
        		for(auto& [sid,cnt]:studentCount){
            		if(cnt==maxStudentCount){
                		Student*s=getStudentByID(sid);
                		if(s)
                    		cout<<" - "<<s->username<<" ("<<cnt<<" reservations)\n";
            		}
        		}
    		}
	 		else{
        		cout<<"No reservations found.\n";
    		}
    		cout<<"----------------------------------------\n";
		}
	}
}
void studentMenu(Student& st){
    while(true){
        cout<<"\n===== Student Menu =====\n";
        cout<<"1. View Food Menu\n";
        cout<<"2. Reserve Food\n";
        cout<<"3. Cancel Reservation\n";
        cout<<"4. My Reservations\n";
        cout<<"5. Add Money\n";
        cout<<"6. Change Password\n";
        cout<<"0. Logout\n";
        int choice;
        if(!readInt(choice)) 
			continue;
        if(choice==0){
			ClearScreen(); 
			return;
		}
        if(choice==1){
            listFoods();
        }
        else if(choice==2){
            if(alreadyReservedToday(st.id)){
                cout<<"Already reserved today.\n";
                continue;
            }
            cout<<"Food ID(0 to cancel): ";
            int fid;
            if(!readInt(fid)) 
				continue;
            Food*f=getFoodByID(fid);
            if(!f){
                cout<<"Invalid food.\n";
                continue;
            }
            if(st.wallet<f->price){
				cout<<"Insufficient balance.\n";
    			continue;
			}
            string qr;
			do{
    		qr=generateQR();
			} 
			while(any_of(reservations.begin(),reservations.end(),
				[&](const Reservation& res){return res.qr==qr;}));
			Reservation r{
    			nextReservationID++,
    			st.id,
    			fid,
    			currentDate(),
    			currentTime(),
    			qr 
			};
            st.wallet-=f->price;
            totalIncome+=f->price;
            reservations.push_back(r);
            saveAll();
            cout<<"Reservation successful.\n";
        }
        else if(choice==3){
            cout<<"Reservation ID(0 to cancel): ";
            int rid;
            if(!readInt(rid)) 
			continue;
            bool found=false;
            for(auto it=reservations.begin();it!=reservations.end();++it){
                 if(it->id==rid){
        			if(it->studentID!=st.id){
            			cout<<"You do not own this reservation.\n";
            			found=true;
            			break;
        			}
                    if(it->date!=currentDate()){
                        cout<<"Cannot cancel past reservations.\n";
                        found=true;
                        break;
                    }
                    Food*f=getFoodByID(it->foodID);
                    if(f){
                        st.wallet+=f->price;
                        totalIncome-=f->price;
                        totalRefund+=f->price;
                    }
                    reservations.erase(it);
                    saveAll();
                    cout<<"Reservation cancelled.\n";
                    found=true;
                    break;
                }
            }
            if(!found)
                cout<<"Reservation not found.\n";
        }
        else if(choice==4){
         	listStudentReservations(st);
        }
        else if(choice==5){
        	cout<<"Current wallet balance: "<<fixed<<setprecision(2)<<st.wallet<<"$\n";
            cout<<"Amount you want to increase(0 to cancel): ";
            double amt;
            if(readDouble(amt) && amt>0){
                st.wallet+=amt;
                saveAll();
                cout<<"Amount added successfully.\nCurrent wallet: "<<st.wallet<<"$\n";
            }
        }
        else if(choice==6){
            cout<<"New password(0 to cancel): ";
            string p;
            cin>>p;
            st.passHash=hashPassword(p);
            saveAll();
            cout<<"Password has been changed successfully.\n";
        }
    }
}
int main(){
    loadAll();
    srand(static_cast<unsigned int>(time(nullptr)));
    while(true){
        cout<<"====== Login Menu ======\n";
        cout<<"Choose who you are entering as:\n";
        cout<<"1. Admin \n";
        cout<<"2. Student \n";
        cout<<"0. Exit\n";
        int choice;
        if(!readInt(choice)) 
			continue;
        if(choice==0) 
			break;
        cout<<"Username: ";
        string user;
		cin.ignore();
        getline(cin,user);
        cout<<"Password: ";
        string pass;
        getline(cin,pass);
        bool loggedIn=false;
        if(choice==1){
            for(auto& a:admins)
                if(a.username==user && passwordMatches(pass,a.passHash)){
                    adminMenu(a);
                    loggedIn=true;
                    break;
                }
        }
        else if(choice==2){
            for(auto& s:students)
                if(s.username==user && passwordMatches(pass,s.passHash)){
                    studentMenu(s);
                    loggedIn=true;
                    break;
                }
        }
        if(!loggedIn){
            cout<<"Invalid login.\n";
            ClearScreen();
		}
    }
    saveAll();
    return 0;
}
