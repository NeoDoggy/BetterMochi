extern bool caseNinit[pages];
extern bool setInit;

void initWithout(int wo,int pg){
  setInit=0;
  for(int i=0;i<pg;i++){
    if(i!=wo)caseNinit[i]=0;
  }
}

void initAll(int pg){
  for(int i=0;i<pg;i++)caseNinit[i]=0;
}