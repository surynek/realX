/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                             realX 0-057_nofutu                             */
/*                                                                            */
/*                  (C) Copyright 2021 - 2022 Pavel Surynek                   */
/*                                                                            */
/*                http://www.surynek.net | <pavel@surynek.net>                */
/*       http://users.fit.cvut.cz/surynek | <pavel.surynek@fit.cvut.cz>       */
/*                                                                            */
/*============================================================================*/
/* waxman_test.cpp / 0-057_nofutu                                             */
/*----------------------------------------------------------------------------*/
//
// Waxman graph testing for virtual network embedding.
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

#include "test/waxman_test.h"


using namespace std;
using namespace realX;


/*----------------------------------------------------------------------------*/

namespace realX
{




/*----------------------------------------------------------------------------*/

void print_Introduction(void)
{
    printf("----------------------------------------------------------------\n");    
    printf("%s : Waxman Graph Test Program\n", sPRODUCT);
    printf("%s\n", sCOPYRIGHT);
    printf("================================================================\n");
}


sResult waxman_test_1(void)
{
    sPathEmbeddingModel path_embedding_Model;
    
    sBoolEncoder encoder;
    Glucose::Solver *solver = path_embedding_Model.setup_SATSolver();
	
    path_embedding_Model.m_physical_Network.generate_UndirectedWaxman(8, 0.5, 0.5, 50, 50);
    path_embedding_Model.m_physical_Network.to_Screen();

    s_DirectedGraph *virtual_network;
    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->generate_DirectedWaxman(8, 0.5, 0.5, 50, 50);
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


sResult waxman_test_2(void)
{
    sPathEmbeddingModel path_embedding_Model;
    
    sBoolEncoder encoder;
    Glucose::Solver *solver = path_embedding_Model.setup_SATSolver();
	
    path_embedding_Model.m_physical_Network.generate_UndirectedWaxman(100, 0.5, 0.5, 50, 50);
    path_embedding_Model.m_physical_Network.to_Screen();

    s_DirectedGraph *virtual_network;
    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->generate_DirectedWaxman(16, 0.5, 0.5, 50, 50);
    virtual_network->to_Screen();

    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->generate_DirectedWaxman(16, 0.5, 0.5, 50, 50);
    virtual_network->to_Screen();

    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->generate_DirectedWaxman(16, 0.5, 0.5, 50, 50);
    virtual_network->to_Screen();

    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->generate_DirectedWaxman(16, 0.5, 0.5, 50, 50);
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

    printf("Building model ...\n");
    path_embedding_Model.setup_LazyPathModel(&encoder);
    //path_embedding_Model.to_Screen();

    path_embedding_Model.build_LazyPathModel(&encoder, solver);
    printf("Building model ... finished\n");    

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
	getchar();
    }
   
    return sRESULT_SUCCESS;
}


sResult waxman_test_3(void)
{
    clock_t time_start, time_finish;

    time_start = clock();
    sPathEmbeddingModel path_embedding_Model;
    
    sBoolEncoder encoder;
    Glucose::Solver *solver = path_embedding_Model.setup_SATSolver();
	
    path_embedding_Model.m_physical_Network.generate_UndirectedWaxman(100, 0.5, 0.5, 50, 50);
    path_embedding_Model.m_physical_Network.to_Screen();

    s_DirectedGraph *virtual_network;
    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->generate_DirectedWaxman(16, 0.5, 0.5, 50, 50);
    virtual_network->to_Screen();

    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->generate_DirectedWaxman(16, 0.5, 0.5, 50, 50);
    virtual_network->to_Screen();

    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->generate_DirectedWaxman(16, 0.5, 0.5, 50, 50);
    virtual_network->to_Screen();

    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->generate_DirectedWaxman(16, 0.5, 0.5, 50, 50);
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
    sInt_32 depth = 10;

    printf("Building model ...\n");
    path_embedding_Model.setup_LimitedLazyPathModel(&encoder, depth);
    //path_embedding_Model.to_Screen();

    path_embedding_Model.build_LimitedLazyPathModel(&encoder, solver, depth);
    printf("Building model ... finished\n");    

    while (true)
    {
	printf("Resolving ...\n");	
	if (path_embedding_Model.solve_LazyPathModel(solver))
	{
	    printf("Virtual network embedding found !\n");
	    
	    sEmbeddingModel::Mappings_vector vertex_Embeddings;
	    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;
	    
	    path_embedding_Model.decode_LimitedLazyPathModel(solver, vertex_Embeddings, path_Embeddings, depth);

	    if (path_embedding_Model.refine_LimitedLazyPathModel(&encoder, solver, vertex_Embeddings, path_Embeddings, depth))
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
    time_finish = clock();
    printf("Total CPU time: %.3f (seconds)\n", (time_finish - time_start) / (sDouble)CLOCKS_PER_SEC);
   
    return sRESULT_SUCCESS;
}


sResult waxman_test_4(void)
{
    clock_t time_start, time_finish;

    time_start = clock();
    sPathEmbeddingModel path_embedding_Model;
    
    sBoolEncoder encoder;
    Glucose::Solver *solver = path_embedding_Model.setup_SATSolver();
	
    path_embedding_Model.m_physical_Network.generate_UndirectedWaxman(100, 0.5, 0.5, 50, 50);
    path_embedding_Model.m_physical_Network.to_Screen();

    s_DirectedGraph *virtual_network;
    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->generate_DirectedWaxman(16, 0.5, 0.5, 50, 50);
    virtual_network->to_Screen();

    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->generate_DirectedWaxman(16, 0.5, 0.5, 50, 50);
    virtual_network->to_Screen();

    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->generate_DirectedWaxman(16, 0.5, 0.5, 50, 50);
    virtual_network->to_Screen();

    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->generate_DirectedWaxman(16, 0.5, 0.5, 50, 50);
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
    sInt_32 depth = 10;
    sDouble geographical_distance = 15.0;

    printf("Building model ...\n");
    path_embedding_Model.setup_LimitedLazyPathModel(&encoder, depth, geographical_distance);
    //path_embedding_Model.to_Screen();

    path_embedding_Model.build_LimitedLazyPathModel(&encoder, solver, depth);
    printf("Building model ... finished\n");    

    while (true)
    {
	printf("Resolving ...\n");	
	if (path_embedding_Model.solve_LazyPathModel(solver))
	{
	    printf("Virtual network embedding found !\n");
	    
	    sEmbeddingModel::Mappings_vector vertex_Embeddings;
	    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;
	    
	    path_embedding_Model.decode_LimitedLazyPathModel(solver, vertex_Embeddings, path_Embeddings, depth);

	    if (path_embedding_Model.refine_LimitedLazyPathModel(&encoder, solver, vertex_Embeddings, path_Embeddings, depth))
	    {
		printf("Model has been refined ...\n");		
	    }
	    else
	    {
		path_embedding_Model.to_Screen_embedding(vertex_Embeddings, path_Embeddings);
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
    time_finish = clock();
    printf("Total CPU time: %.3f (seconds)\n", (time_finish - time_start) / (sDouble)CLOCKS_PER_SEC);
   
    return sRESULT_SUCCESS;
}


}  // namespace realX


/*----------------------------------------------------------------------------*/

int main(int sUNUSED(argc), const char **sUNUSED(argv))
{
    sResult result;

    print_Introduction();
/*
    if (sFAILED(result = waxman_test_1()))
    {
	printf("Test waxman 1 failed (error:%d).\n", result);
	return result;
    }
*/
/*    
    if (sFAILED(result = waxman_test_2()))
    {
	printf("Test waxman 2 failed (error:%d).\n", result);
	return result;
    }         
*/
/*
    if (sFAILED(result = waxman_test_3()))
    {
	printf("Test waxman 3 failed (error:%d).\n", result);
	return result;
    }
*/
    if (sFAILED(result = waxman_test_4()))
    {
	printf("Test waxman 4 failed (error:%d).\n", result);
	return result;
    }                 
   
    return sRESULT_SUCCESS;
}
    
