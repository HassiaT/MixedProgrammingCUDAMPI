#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <ctime>
#include <vector>
#include <math.h>
#include <random>


using namespace std;
/* BINOMIAL METHOD */
/*** PRICING D'UN CALL EUROPEEN ***/
double pricingCall_E(double r, double k, double d, double u, double S, int n)
{
  double R  = 1+r;
  double Pu = (R-d)/(u-d);
  double Pd = (u-R)/(u-d);

  /* declaration de l'arbre */
  vector< vector<double> > treilli(n+1);

  /*** INITIALISATION DU VECTEUR ***/
  for (int i=0; i<=n;i++) {
    vector<double> vec(i+1);
    treilli.insert(treilli.begin()+i, vec);
  }
  
  for (int i=n;i>=0;i--) {
    int size = i+1;
    vector<double>prices(size);
    
    /* NOEUD TERMINAL */
    if ( i == n ) {
      for (int j= prices.size()-1; j>=0;j--) {
	double price_int = ((pow(u,(double)j)*pow(d,(double)(i-j))*S)- k);
	prices[j] = (price_int > 0)? price_int : 0;
	treilli[i][j] = prices[j];
	//cout<<treilli[i][j]<<endl;
      }
      prices.clear();
    }
    
    /* NOEUD NON TERMINAL */
    else {
      for (int j=prices.size()-1;j>=0;j--) {
	double p1 = (1/R) * (Pu* treilli[i+1][j] + Pd*treilli[i+1][j+1]);
	double p2 = ((pow(u,(double)j)*pow(d,(double)(i-j))*S)- k);
	prices[j]= (p1 > p2)? p1 : p2;
	treilli[i][j] = prices[j];
      }
      prices.clear();
    }
  }
  return (treilli[0][0]);
}

/*** PRICING D'UN PUT EUROPEEN ***/
double pricingPut_E(double r, double k, double d, double u, double S, int n)
{
  double R  = 1+r;
  double Pu = (R-d)/(u-d);
  double Pd = (u-R)/(u-d);
  
  /* declaration de l'arbre */
  vector< vector<double> > treilli(n+1);
  
  /*** INITIALISATION DU VECTEUR ***/
  for (int i=0; i<=n;i++) {
    vector<double> vec(i+1);
    treilli.insert(treilli.begin()+i, vec);
  }
  
  for (int i=n;i>=0;i--) {
    int size =i+1;
    vector<double>prices(size);
    
    /* NOEUD TERMINAL */
    if ( i == n) {
      for (int j= prices.size()-1; j>=0;j--) {
	double price_int = (k-(pow(u,(double)j)*pow(d,(double)(i-j))*S));
	prices[j] = (price_int > 0)? price_int : 0;
	treilli[i][j] = prices[j];
      }
      prices.clear();
    }

    /* NOEUD NON TERMINAL */
    else {
      for (int j=prices.size()-1;j>=0;j--) {
	double p1 = (1/R) * (Pu* treilli[i+1][j] + Pd*treilli[i+1][j+1]);
	double p2 = (k-(pow(u,(double)j) * pow(d, (double)(i-j))*S));
	prices[j] = (p1 > p2)? p1 : p2;
	treilli[i][j] = prices[j];
      }
      prices.clear();
    }
  }
  return (treilli[0][0]);
}

/*** FONCTION TEST ***/
void afficher_treilli(vector <vector<double> > treilli, int size) {
  for (int i=size-1; i>=0;i--) {
    if (i == size-1) cout<< "PRIX TERMINAUX "<<endl;
    cout<<"i :"<<i<<endl;
    int n = treilli[i].size();
    for (int j=n-1; j>=0; j--) {
      cout<<"j   :"<<j<<"    "<<treilli[i][j] <<" ";
    }
    cout<<"\n";
  }
}


/*** PRICING D'UN PUT AMERICAIN ***/
double pricingPut_A(double r, double k, double d, double u, double S, int n)
{
  double R  = 1+r;
  double Pu = (R-d)/(u-d);
  double Pd = (u-R)/(u-d);

  /* declaration de l'arbre */
  vector< vector<double> > treilli(n+1);

  /*** INITIALISATION DU VECTEUR ***/
  for (int i=0; i<=n;i++) {
    vector<double> vec(i+1);
    treilli.insert(treilli.begin()+i, vec);
  }

  for (int i=n;i>=0;i--) {
    int size = i+1;
    vector<double>prices(size);
    
    /* NOEUD TERMINAL */
    if ( i == n) {
      for (int j= size-1; j>=0; j--) {
	double price_int = (k-(pow(u,(double)j)*pow(d,(double)(i-j))*S));
	prices[j] = (price_int > 0)? price_int : 0;
	treilli[i][j] = prices[j];
	//cout<<prices[j]<<endl;
      }
      prices.clear();
    }
    
    /* NOEUD NON TERMINAL */
    else {
      for (int j=size-1;j>=0;j--) {
	double p1 = (1/R) * (Pu*treilli[i+1][j] + Pd*treilli[i+1][j+1]);
	double p2 = (k - (pow(u, (double)j)*pow(d,(double)(i-j))*S));
       	prices[j] = (p1 > p2)? p1 : p2;
	treilli[i][j] = prices[j];
	//cout<<prices[j]<<endl;
      }
      prices.clear();
    }
  }
  return (treilli[0][0]);
}


/*** PRICING D'UN CALL AMERICAIN ***/
double pricingCall_A(double r, double k, double d, double u,double S, int n)
{
  double R  = 1+r;
  double Pu = (R-d)/(u-d);
  double Pd = (u-R)/(u-d);

  /* declaration de l'arbre */
  vector< vector<double> > treilli(n+1);

  /*** INITIALISATION DU VECTEUR ***/
  for (int i=0; i<=n;i++) {
    vector<double> vec(i+1);
    treilli.insert(treilli.begin()+i, vec);
  }
  
  for (int i=n;i>=0;i--) {
    int size = i+1;
    vector<double>prices(size);
    
    /* NOEUD TERMINAL */
    if ( i == n ) {
      for (int j= prices.size()-1; j>=0;j--) {
	double price_int = ( (pow(u,(double)j)*pow(d,(double)(i-j))*S) - k);
	prices[j] = (price_int > 0)? price_int : 0;
	treilli[i][j] = prices[j];
      }
    }
    
    /* NOEUD NON TERMINAL */
    else {
      for (int j=prices.size()-1;j>=0;j--) {
	double p1 = (1/R) * (Pu* treilli[i+1][j] + Pd*treilli[i+1][j+1]);
	double p2 = (pow(u, (double)j)*pow(d,(double)(i-j))*S)-k;
	prices[j] = (p1 > p2)? p1 : p2;
	treilli[i][j] =  prices[j];
	//cout<<prices[j]<<endl;
      }
      prices.clear();
    }
  }
  return (treilli[0][0]);
}

void lancer_pricing()
{
  int  choix_opt;
  char choix_nat;
  int nbre_periodes;

  /* K : strike - prix d'exercice */
  /* r : taux de crédit */
  /* d et u : paramètres risque de probabilité neutre */
  double K,r,d,u,S; 
  
  cout<<"Entrer le nombre de periodes :"<<endl;
  cin>>nbre_periodes;
  cout<<"\n";
  
  cout<<"Entrer le prix initial :"<<endl;
  cin>>S;
  cout<<"\n";
  
  cout<<"Entrer le prix d'exercice de l'option "<<endl;
  cin>>K;
  cout<<"\n";
  
  cout<<"Entrer le taux de crédit "<<endl;
  cin>>r;
  cout<<"\n";

  cout<<"Entrer le paramètre d "<<endl;
  cin>>d;
  cout<<"\n";

  cout<<"Entrer le paramètre u "<<endl;
  cin>>u;
  cout<<"\n";

  cout<<"CHOISISSEZ LA NATURE DE L'OPTION :"<<endl;
  cout<<"\n";
  cout<<"1 -- CALL/OPTION D'ACHAT"<<endl;
  cout<<"2 -- PUT/OPTION DE VENTE"<<endl;
  cout<<"\n";
  cin>>choix_opt;

  cout<<"CHOISISSEZ LE TYPE DE L'OPTION :"<<endl;
  cout<<"\n";
  cout<<"A -- AMERICAINE "<<endl;
  cout<<"E -- EUROPEENNE "<<endl;
  cout<<"\n";
  cin>>choix_nat;

  /* l'utilisateur a choisi un CALL */
  if ( choix_opt == 1) {
    if (choix_nat =='E') {
      cout<<"\n";
      cout<<"PRICING D'UN CALL EUROPEEN "<<endl;
      cout<<"\n";
      cout<<pricingCall_E(r,K,d,u,S,nbre_periodes)<<endl;
      cout<<"\n";
    }
    else if (choix_nat =='A') {
      cout<<"\n";
      cout<<"PRICING D'UN CALL AMERICAIN "<<endl;
      cout<<"\n";
      cout<<pricingCall_A(r,K,d,u,S,nbre_periodes)<<endl;
      cout<<"\n";
    }
    else cout<<"ERREUR 1"<<endl;
  }

  /* l'utilisateur a choisi un PUT */
  else if ( choix_opt == 2) {
    if (choix_nat =='E') {
      cout<<"\n";
      cout<<"PRICING D'UN PUT EUROPEEN "<<endl;
      cout<<"\n";
      cout<<pricingPut_E(r,K,d,u,S,nbre_periodes)<<endl;
      cout<<"\n";
    }
    else if (choix_nat =='A') {
      cout<<"\n";
      cout<<"PRICING D'UN PUT AMERICAIN "<<endl;
      cout<<"\n";
      cout<<pricingPut_A(r,K,d,u,S,nbre_periodes)<<endl;
      cout<<"\n";
    }
    else cout<<"ERREUR 2"<<endl;
  }

  else cout<<"ERREUR 3"<<endl;
}

/*** INITIALISATION DU FICHIER CONTENANT LES OPTIONS ***/
void init_option(string file) {
  random_device rd;
  default_random_engine generator(rd());
  double r,k,d,u,S;
  
  /* taux de credit */
  std::uniform_real_distribution<double> credit(0.03, 0.09);
  /* le strike */
  std::uniform_real_distribution<double> strike(60.0,120.0);

  /* parametre d */
  std::uniform_real_distribution<double> param_d(1.0020, 3.0157);
  
  /* parametre u */
  std::uniform_real_distribution<double> param_u(0.91208, 0.9838);

  /* prix initial */
  std::uniform_real_distribution<double> prix(90.0,115.0);
  
  ofstream fichier(file.c_str(), ios::in | ios::trunc );
  
  /* ouverture du fichier */
  if ( fichier ) {

    /* on initialise les paramètres de 200 options */
    for ( int opt=0; opt<200; opt++ ) {
      r = credit(generator);
      k = strike(generator);
      d = param_d(generator);
      u = param_u(generator);
      S = prix(generator);

      fichier << r <<" " <<k <<" "<<d <<" "<<u <<" "<<S <<"\n";
    }
    fichier.close();
  }

  else {
    cerr <<"Impossible d'ouvrir le fichier ! "<<endl;
  }

}

/*** PRICING D'OPTIONS A PARTIR D'UN FICHIER ***/
/**  nbre_opt : nombre d'options   **/
/**  periodes : nombre de periodes **/
/**  file : nom du fichier **/
/**  type : représente la nature de l'option **/

 
/** type : 0 CALL EUROPEEN **/
/** type : 1 PUT EUROPEEN **/
/** type : 2 CALL AMERICAIN **/
/** type : 3 PUT AMERICAIN **/
double *pricing_option (int type, int nbre_opt, int periodes, string file) {
  double *prices = new double[nbre_opt];
  double r,k,d,u,S;

  ifstream fichier(file, ios::in);

  if ( fichier ) {
    
    /* LIRE LE FICHIER LIGNE PAR LIGNE */
    for (int i=0 ; i<nbre_opt ; i++) {
      fichier >> r >> k >> d >> u >> S;
      /** TEST AFFICHAGE **/
      //cout<< "r : "<<r<<endl;
      //cout<< "k : "<<k<<endl;
      //cout<< "d : "<<d<<endl;
      //cout<< "u : "<<u<<endl;
      //cout<< "S : "<<S<<endl;
      
      /* CALL EUROPEEN */
      if ( type == 0) prices[i] = pricingCall_E(r,k,d,u,S,periodes);

      /* PUT EUROPEEN */
      if ( type == 1) prices[i] = pricingPut_E(r,k,d,u,S,periodes); 
      
      /* CALL AMERICAIN */
      if ( type == 2) prices[i] = pricingCall_A(r,k,d,u,S,periodes);
    
    /* PUT AMERICAIN */
      if ( type == 3) prices[i] = pricingPut_A(r,k,d,u,S,periodes);
    }
  fichier.close();
  }

  else {
    cerr << " Impossible d'ouvrir le fichier ! "<<endl;
  }
  return prices;
}

 
int main (int argc, char* argv[])
{
  int init;
  string file="option.txt";

  
  cout<<"METHODE DE PRICING :"<<endl;
  cout<<"\n";
  cout<<"1 -- INITIALISATION MANUELLE "<<endl;
  cout<<"2 -- INITIALISATION AUTOMATIQUE "<<endl;
  cout<<"\n";
  cin>>init;

  if (init == 1 ) {
    lancer_pricing();
  }
  
  else if ( init == 2 ) {
    init_option(file);
    cout<<"\n";
    cout<<"PREMIER TEST : PRICING 50 OPTIONS CALL AMERICAIN "<<endl;
    cout<<"NOMBRE DE PERIODES : 4 "<<endl;
    clock_t beg1 = clock();
    pricing_option(2,50,4,file);
    clock_t end1 = clock();
    double elapsed1 = double (end1-beg1) /CLOCKS_PER_SEC;
    cout<<elapsed1<<endl;
    cout<<"\n";

    cout<<"SECOND TEST : PRICING 100 OPTIONS CALL AMERICAIN "<<endl;
    cout<<"NOMBRE DE PERIODES : 8 "<<endl;
    clock_t beg2 = clock();
    pricing_option(2,100,8,file);
    clock_t end2 = clock();
    double elapsed2 = double (end2-beg2) /CLOCKS_PER_SEC;
    cout<<elapsed2<<endl;
    cout<<"\n";

    cout<<"TROISIEME TEST : PRICING 100 OPTIONS CALL AMERICAIN "<<endl;
    cout<<"NOMBRE DE PERIODES : 16 "<<endl;
    clock_t beg3 = clock();
    pricing_option(2,100,16,file);
    clock_t end3 = clock();
    double elapsed3 = double (end3-beg3) /CLOCKS_PER_SEC;
    cout<<elapsed3<<endl;
    cout<<"\n";
    
    cout<<"QUATRIEME TEST : PRICING 100 OPTIONS CALL AMERICAIN "<<endl;
    cout<<"NOMBRE DE PERIODES : 24 "<<endl;
    clock_t beg4 = clock();
    pricing_option(2,100,24,file);
    clock_t end4 = clock();
    double elapsed4 = double (end4-beg4) /CLOCKS_PER_SEC;
    cout<<elapsed4<<endl;
    cout<<"\n";



    
  }
  return EXIT_SUCCESS;
}
 
