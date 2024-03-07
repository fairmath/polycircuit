#include "matrix_mult/ckks.hpp"
int n=64;
int n_log=6;

MatrixMultCKKS::MatrixMultCKKS(std::string ccLocation, std::string pubKeyLocation, std::string multKeyLocation,
                               std::string rotKeyLocation,
                               std::string matrixALocation,
                               std::string matrixBLocation,
                               std::string outputLocation) : m_PubKeyLocation(pubKeyLocation),
                                                             m_MultKeyLocation(multKeyLocation),
                                                             m_RotKeyLocation(rotKeyLocation),
                                                             m_CCLocation(ccLocation),
                                                             m_MatrixALocation(matrixALocation),
                                                             m_MatrixBLocation(matrixBLocation),
                                                             m_OutputLocation(outputLocation)
{

    initCC();
};

void MatrixMultCKKS::initCC()
{

    if (!Serial::DeserializeFromFile(m_CCLocation, m_cc, SerType::BINARY))
    {
        std::cerr << "Could not deserialize cryptocontext file" << std::endl;
        std::exit(1);
    }
    

    

    if (!Serial::DeserializeFromFile(m_PubKeyLocation, m_PublicKey, SerType::BINARY))
    {
        std::cerr << "Could not deserialize public key file" << std::endl;
        std::exit(1);
    }
    


    std::ifstream multKeyIStream(m_MultKeyLocation, std::ios::in | std::ios::binary);
    if (!multKeyIStream.is_open())
    {
        std::exit(1);
    }
 
    
    if (!m_cc->DeserializeEvalMultKey(multKeyIStream, SerType::BINARY))
    {
        std::cerr << "Could not deserialize rot key file" << std::endl;
        std::exit(1);
    }
    
    


    std::ifstream rotKeyIStream(m_RotKeyLocation, std::ios::in | std::ios::binary);
    if (!rotKeyIStream.is_open())
    {
        std::exit(1);
    }


    if (!m_cc->DeserializeEvalAutomorphismKey(rotKeyIStream, SerType::BINARY))
    {
        std::cerr << "Could not deserialize eval rot key file" << std::endl;
        std::exit(1);
    }
    
    
    

    if (!Serial::DeserializeFromFile(m_MatrixALocation, m_MatrixAC, SerType::BINARY))
    {
        std::cerr << "Could not deserialize Matrix A cipher" << std::endl;
        std::exit(1);
    }
    


    if (!Serial::DeserializeFromFile(m_MatrixBLocation, m_MatrixBC, SerType::BINARY))
    {
        std::cerr << "Could not deserialize Matrix B cipher" << std::endl;
        std::exit(1);
    }
 
    
}

void MatrixMultCKKS::eval()
{
    // You should implement this function

    //---------------------- Multiplication evaluation -----------------------------------
    
    
    
    std::vector<double> mask1(2*n*n,0);
    std::vector<std::vector<double>> mask;
    mask.push_back(mask1);
    mask.push_back(mask1);
    
    
    std::vector<Ciphertext<DCRTPoly>> out(32);
    std::vector<Plaintext> plaintext_mask(2);
    
   
    #pragma omp parallel for collapse(2)
    for(int k=0;k<4;k++){
		        for(int i=0;i<n;i++){
		                int t=(k%2)*(i)+(1-(k%2))*(i*n);
			    	mask[(k%2)][t+(int)(k/2)*n*n]=1 ;
			    }
		    }
    
    
    #pragma omp parallel for
    for(int j=0;j<2;j++){
    	plaintext_mask[j]  = m_cc->MakeCKKSPackedPlaintext(mask[j]);
    }
    	
    
   
    
    #pragma omp parallel sections
    {   
	    #pragma omp section
	    m_MatrixAC=m_cc->EvalAdd(m_MatrixAC,m_cc->EvalRotate(m_MatrixAC,-n*n+1));
	    #pragma omp section
	    m_MatrixBC=m_cc->EvalAdd(m_MatrixBC,m_cc->EvalRotate(m_MatrixBC,-n*n+n));
    }
    
     #pragma omp parallel for shared(m_MatrixAC,m_MatrixBC,plaintext_mask)
    for(int t=0;t<(int)(n/2);t++){
        std::vector<Ciphertext<DCRTPoly>> ab1(2),ab2(2);
	    if(t!=0){
		        
		        ab1[0]=m_cc->EvalRotate(m_MatrixAC,2*t);
		        ab1[1]=m_cc->EvalRotate(m_MatrixBC,2*t*n);
		         
	     }
	    else{
		    ab1[0]=m_MatrixAC;
		    ab1[1]=m_MatrixBC;
	        
	     }
 
          //#pragma omp parallel for
	 for(int j=0;j<2;j++){
	    	ab1[j]  =m_cc->EvalMult(ab1[j],plaintext_mask[j]);
	    	for(int k=0;k<n_log;k++){
		    	int l=-1*pow(2,k);
		    	ab2[j]=m_cc->EvalRotate(ab1[j],l*pow(n,j));
			ab1[j]=m_cc->EvalAdd(ab1[j],ab2[j]);	 
		    }
	 }
	      
	    
	     out[t]=m_cc->EvalMult(ab1[0],ab1[1]);
	       
    }
    
			   
	    for(int i=1;i<=(int)(log2(n/2));i++){
            #pragma omp parallel for 
	        for(int t=0;t<(int)(n/pow(2,i+1));t++){
                m_cc->EvalAddInPlace(out[t], out[t+(int)(n/pow(2,i+1))]);
            }
        }
            
   out[0]=m_cc->EvalAdd(out[0],m_cc->EvalRotate(out[0],n*n));   

    m_OutputC=out[0];
    

   
    
    //---------------------- Multiplication evaluation done -----------------------------------
       
   
    
}

void MatrixMultCKKS::deserializeOutput()
{
    
    if (!Serial::SerializeToFile(m_OutputLocation, m_OutputC, SerType::BINARY))
    {
        std::cerr << " Error writing ciphertext 1" << std::endl;
    }
}
