/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                             realX 0-094_nofutu                             */
/*                                                                            */
/*                  (C) Copyright 2021 - 2022 Pavel Surynek                   */
/*                                                                            */
/*                http://www.surynek.net | <pavel@surynek.net>                */
/*       http://users.fit.cvut.cz/surynek | <pavel.surynek@fit.cvut.cz>       */
/*                                                                            */
/*============================================================================*/
/* embedding_test.cpp / 0-094_nofutu                                          */
/*----------------------------------------------------------------------------*/
//
// Virtual network embedding test.
//
/*----------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <signal.h>
#include <glpk.h>

#include "defs.h"
#include "compile.h"
#include "result.h"
#include "version.h"

#include "common/types.h"
#include "common/graph.h"

#include "core/robot.h"
#include "core/network.h"
#include "core/cnf.h"
#include "util/geometry.h"

#include "test/embedding_test.h"


using namespace std;
using namespace realX;


/*----------------------------------------------------------------------------*/

namespace realX
{




/*----------------------------------------------------------------------------*/

void print_Introduction(void)
{
    printf("----------------------------------------------------------------\n");    
    printf("%s : Virtual Network Embedding Test Program\n", sPRODUCT);
    printf("%s\n", sCOPYRIGHT);
    printf("================================================================\n");
}


sResult embedding_test_1(void)
{
    sEmbeddingModel embedding_Model;
    
    sBoolEncoder encoder;
    Glucose::Solver *solver = embedding_Model.setup_SATSolver();
	
    embedding_Model.m_physical_Network.add_Vertex(1.0);
    embedding_Model.m_physical_Network.add_Vertex(2.0);
    embedding_Model.m_physical_Network.add_Vertex(2.0);
    embedding_Model.m_physical_Network.add_Vertex(1.0);

    embedding_Model.m_physical_Network.add_Arrow(0, 1, 1.0);
    embedding_Model.m_physical_Network.add_Arrow(0, 2, 1.0);
    embedding_Model.m_physical_Network.add_Arrow(1, 2, 2.0);
    embedding_Model.m_physical_Network.add_Arrow(1, 3, 1.0);
    embedding_Model.m_physical_Network.add_Arrow(2, 3, 1.0);    

    embedding_Model.m_physical_Network.to_Screen();

    s_DirectedGraph *virtual_network;
    embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &embedding_Model.m_virtual_Networks.back();

    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);

    virtual_network->add_Arrow(1, 0, 1.0);
    virtual_network->add_Arrow(1, 2, 1.0);
    virtual_network->add_Arrow(0, 2, 1.0);

    virtual_network->to_Screen();

    embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &embedding_Model.m_virtual_Networks.back();

    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);

    virtual_network->add_Arrow(0, 1, 1.0);
    virtual_network->add_Arrow(0, 2, 1.0);
    virtual_network->add_Arrow(1, 2, 1.0);

    virtual_network->to_Screen();    

    embedding_Model.setup_LazyModel(&encoder);
    embedding_Model.to_Screen();

    embedding_Model.build_LazyModel(&encoder, solver);

    while (true)
    {
	printf("Resolving ...\n");
	if (embedding_Model.solve_LazyModel(solver))
	{
	    printf("Virtual network embedding found !\n");
	    
	    sEmbeddingModel::Mappings_vector virtual_Embeddings;	
	    embedding_Model.decode_LazyModel(solver, virtual_Embeddings);

	    if (embedding_Model.refine_LazyModel(&encoder, solver, virtual_Embeddings))
	    {
		printf("Model has been refined ...\n");		
	    }
	    else
	    {
		printf("Finally a correct embedding has been found !\n");
		break;
	    }
	}
	else
	{
	    printf("Embedding does NOT exist.\n");
	    break;
	}
    }
    
    
    return sRESULT_SUCCESS;
}


sResult embedding_test_2(void)
{
    sPathEmbeddingModel path_embedding_Model;
    
    sBoolEncoder encoder;
    Glucose::Solver *solver = path_embedding_Model.setup_SATSolver();
	
    path_embedding_Model.m_physical_Network.add_Vertex(1.0);
    path_embedding_Model.m_physical_Network.add_Vertex(2.0);
    path_embedding_Model.m_physical_Network.add_Vertex(2.0);
    path_embedding_Model.m_physical_Network.add_Vertex(1.0);

    path_embedding_Model.m_physical_Network.add_Arrow(0, 1, 1.0);
    path_embedding_Model.m_physical_Network.add_Arrow(0, 2, 1.0);
    path_embedding_Model.m_physical_Network.add_Arrow(1, 2, 2.0);

    path_embedding_Model.m_physical_Network.to_Screen();

    s_DirectedGraph *virtual_network;
    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);

    virtual_network->add_Arrow(1, 0, 1.0);
    virtual_network->add_Arrow(1, 2, 1.0);
    virtual_network->add_Arrow(0, 2, 1.0);

    virtual_network->to_Screen();
/*
    embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &embedding_Model.m_virtual_Networks.back();

    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);

    virtual_network->add_Arrow(0, 1, 1.0);
    virtual_network->add_Arrow(0, 2, 1.0);
    virtual_network->add_Arrow(1, 2, 1.0);

    virtual_network->to_Screen();    
*/
    path_embedding_Model.setup_LazyPathModel(&encoder);
    path_embedding_Model.to_Screen();

    path_embedding_Model.build_LazyPathModel(&encoder, solver);

    while (true)
    {
	printf("Resolving ...\n");	
	if (path_embedding_Model.solve_LazyPathModel(solver))
	{
	    printf("Virtual network embedding found !\n");
	    
	    sPathEmbeddingModel::Mappings_vector vertex_Embeddings;
	    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;
	    
	    path_embedding_Model.decode_LazyPathModel(solver, vertex_Embeddings, path_Embeddings);
/*
	    if (path_embedding_Model.refine_LazyModel(&encoder, solver, virtual_Embeddings))
	    {
		printf("Model has been refined ...\n");		
	    }
	    else
	    {
		printf("Finally a correct embedding has been found !\n");
		break;
	    }
*/
	}
	else
	{
	    printf("Embedding does NOT exist.\n");
	    break;
	}
	break;
    }
    
    
    return sRESULT_SUCCESS;
}    



sResult embedding_test_3(void)
{
    sPathEmbeddingModel path_embedding_Model;
    
    sBoolEncoder encoder;
    Glucose::Solver *solver = path_embedding_Model.setup_SATSolver();
	
    path_embedding_Model.m_physical_Network.add_Vertex(1.0);
    path_embedding_Model.m_physical_Network.add_Vertex(1.0);
    path_embedding_Model.m_physical_Network.add_Vertex(1.0);
    path_embedding_Model.m_physical_Network.add_Vertex(1.0);
    path_embedding_Model.m_physical_Network.add_Vertex(1.0);
    path_embedding_Model.m_physical_Network.add_Vertex(1.0);    

    path_embedding_Model.m_physical_Network.add_Arrow(0, 5, 1.0);
    path_embedding_Model.m_physical_Network.add_Arrow(5, 4, 1.0);
    path_embedding_Model.m_physical_Network.add_Arrow(4, 3, 1.0);
    path_embedding_Model.m_physical_Network.add_Arrow(3, 2, 1.0);
    path_embedding_Model.m_physical_Network.add_Arrow(2, 1, 1.0);
    path_embedding_Model.m_physical_Network.add_Arrow(1, 0, 1.0);            

    path_embedding_Model.m_physical_Network.to_Screen();

    s_DirectedGraph *virtual_network;
    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->add_Vertex(2.0);
    virtual_network->add_Vertex(2.0);
    virtual_network->add_Vertex(2.0);

    virtual_network->add_Arrow(0, 2, 1.0);
    virtual_network->add_Arrow(2, 1, 1.0);
    virtual_network->add_Arrow(1, 0, 1.0);

    virtual_network->to_Screen();
/*
    embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &embedding_Model.m_virtual_Networks.back();

    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);

    virtual_network->add_Arrow(0, 1, 1.0);
    virtual_network->add_Arrow(0, 2, 1.0);
    virtual_network->add_Arrow(1, 2, 1.0);

    virtual_network->to_Screen();    
*/
    path_embedding_Model.setup_LazyPathModel(&encoder);
    path_embedding_Model.to_Screen();

    path_embedding_Model.build_LazyPathModel(&encoder, solver);

    while (true)
    {
	printf("Resolving ...\n");	
	if (path_embedding_Model.solve_LazyPathModel(solver))
	{
	    printf("Virtual network embedding found !\n");
	    
	    sEmbeddingModel::Mappings_vector vertex_Embeddings;
	    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;
	    
	    path_embedding_Model.decode_LazyPathModel(solver, vertex_Embeddings, path_Embeddings);

	    if (path_embedding_Model.refine_LazyPathModel(&encoder, solver, vertex_Embeddings, path_Embeddings))
	    {
		printf("Model has been refined ...\n");		
	    }
	    else
	    {
		printf("Finally a correct embedding has been found !\n");
		break;
	    }
	}
	else
	{
	    printf("Embedding does NOT exist.\n");
	    break;
	}
    }
    
    return sRESULT_SUCCESS;
}    


sResult embedding_test_4(void)
{
    sPathEmbeddingModel path_embedding_Model;
    
    sBoolEncoder encoder;
    Glucose::Solver *solver = path_embedding_Model.setup_SATSolver();
	
    path_embedding_Model.m_physical_Network.add_Vertex(1.0);
    path_embedding_Model.m_physical_Network.add_Vertex(2.0);
    path_embedding_Model.m_physical_Network.add_Vertex(2.0);
    path_embedding_Model.m_physical_Network.add_Vertex(1.0);
    path_embedding_Model.m_physical_Network.add_Vertex(2.0);
    path_embedding_Model.m_physical_Network.add_Vertex(1.0);    

    path_embedding_Model.m_physical_Network.add_Arrow(0, 2, 1.0);
    path_embedding_Model.m_physical_Network.add_Arrow(2, 1, 1.0);
    path_embedding_Model.m_physical_Network.add_Arrow(1, 0, 2.0);
    path_embedding_Model.m_physical_Network.add_Arrow(0, 3, 2.0);
    path_embedding_Model.m_physical_Network.add_Arrow(1, 4, 2.0);
    path_embedding_Model.m_physical_Network.add_Arrow(2, 5, 2.0);            

    path_embedding_Model.m_physical_Network.to_Screen();

    s_DirectedGraph *virtual_network;
    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);

    virtual_network->add_Arrow(0, 2, 1.0);
    virtual_network->add_Arrow(2, 1, 1.0);
    virtual_network->add_Arrow(1, 0, 1.0);

    virtual_network->to_Screen();
/*
    embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &embedding_Model.m_virtual_Networks.back();

    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);

    virtual_network->add_Arrow(0, 1, 1.0);
    virtual_network->add_Arrow(0, 2, 1.0);
    virtual_network->add_Arrow(1, 2, 1.0);

    virtual_network->to_Screen();    
*/
    path_embedding_Model.setup_LazyPathModel(&encoder);
    path_embedding_Model.to_Screen();

    path_embedding_Model.build_LazyPathModel(&encoder, solver);

    while (true)
    {
	printf("Resolving ...\n");	
	if (path_embedding_Model.solve_LazyPathModel(solver))
	{
	    printf("Virtual network embedding found !\n");
	    
	    sEmbeddingModel::Mappings_vector vertex_Embeddings;
	    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;
	    
	    path_embedding_Model.decode_LazyPathModel(solver, vertex_Embeddings, path_Embeddings);

	    if (path_embedding_Model.refine_LazyPathModel(&encoder, solver, vertex_Embeddings, path_Embeddings))
	    {
		printf("Model has been refined ...\n");		
	    }
	    else
	    {
		printf("Finally a correct embedding has been found !\n");
		break;
	    }
	}
	else
	{
	    printf("Embedding does NOT exist.\n");
	    break;
	}
	break;
    }
    
    
    return sRESULT_SUCCESS;
}    


sResult embedding_test_5(void)
{
    sPathEmbeddingModel path_embedding_Model;
    
    sBoolEncoder encoder;
    Glucose::Solver *solver = path_embedding_Model.setup_SATSolver();
	
    path_embedding_Model.m_physical_Network.add_Vertex(1.0);
    path_embedding_Model.m_physical_Network.add_Vertex(1.0);
    path_embedding_Model.m_physical_Network.add_Vertex(1.0);
    path_embedding_Model.m_physical_Network.add_Vertex(1.0);

    path_embedding_Model.m_physical_Network.add_Arrow(0, 1, 2.0);
    path_embedding_Model.m_physical_Network.add_Arrow(1, 2, 2.0);
    path_embedding_Model.m_physical_Network.add_Arrow(2, 3, 2.0);
    path_embedding_Model.m_physical_Network.add_Arrow(3, 0, 2.0);
    path_embedding_Model.m_physical_Network.add_Arrow(1, 3, 1.0);

    path_embedding_Model.m_physical_Network.to_Screen();

    s_DirectedGraph *virtual_network;
    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);

    virtual_network->add_Arrow(0, 2, 2.0);
    virtual_network->add_Arrow(2, 1, 2.0);
    virtual_network->add_Arrow(1, 0, 2.0);

    virtual_network->to_Screen();
/*
    embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &embedding_Model.m_virtual_Networks.back();

    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);
    virtual_network->add_Vertex(1.0);

    virtual_network->add_Arrow(0, 1, 1.0);
    virtual_network->add_Arrow(0, 2, 1.0);
    virtual_network->add_Arrow(1, 2, 1.0);

    virtual_network->to_Screen();    
*/
    path_embedding_Model.setup_LazyPathModel(&encoder);
    path_embedding_Model.to_Screen();

    path_embedding_Model.build_LazyPathModel(&encoder, solver);

    while (true)
    {
	printf("Resolving ...\n");	
	if (path_embedding_Model.solve_LazyPathModel(solver))
	{
	    printf("Virtual network embedding found !\n");
	    
	    sEmbeddingModel::Mappings_vector vertex_Embeddings;
	    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;
	    
	    path_embedding_Model.decode_LazyPathModel(solver, vertex_Embeddings, path_Embeddings);

	    if (path_embedding_Model.refine_LazyPathModel(&encoder, solver, vertex_Embeddings, path_Embeddings))
	    {
		printf("Model has been refined ...\n");		
	    }
	    else
	    {
		printf("Finally a correct embedding has been found !\n");
		break;
	    }
	}
	else
	{
	    printf("Embedding does NOT exist.\n");
	    break;
	}
    }
    
    return sRESULT_SUCCESS;
}    


}  // namespace realX


/*----------------------------------------------------------------------------*/

int main(int sUNUSED(argc), const char **sUNUSED(argv))
{
    sResult result;

    print_Introduction();

    /*
    if (sFAILED(result = embedding_test_1()))
    {
	printf("Test embedding 1 failed (error:%d).\n", result);
	return result;
    }
    */

    /*
    if (sFAILED(result = embedding_test_2()))
    {
	printf("Test embedding 2 failed (error:%d).\n", result);
	return result;
    } 
    */
    /*    
    if (sFAILED(result = embedding_test_3()))
    {
	printf("Test embedding 3 failed (error:%d).\n", result);
	return result;
    }
    */
    /*
    if (sFAILED(result = embedding_test_4()))
    {
	printf("Test embedding 4 failed (error:%d).\n", result);
	return result;
    } 
    */
    if (sFAILED(result = embedding_test_5()))
    {
	printf("Test embedding 5 failed (error:%d).\n", result);
	return result;
    }     
   
    return sRESULT_SUCCESS;
}
