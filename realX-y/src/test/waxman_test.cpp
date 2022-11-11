/*============================================================================*/
/*                                                                            */
/*                                                                            */
<<<<<<< HEAD
/*                             realX 0-107_nofutu                             */
=======
/*                             realX 0-104_nofutu                             */
>>>>>>> 14f14b75c8f1d7815a0fb722e1fd0403f95fb68c
/*                                                                            */
/*                  (C) Copyright 2021 - 2022 Pavel Surynek                   */
/*                                                                            */
/*                http://www.surynek.net | <pavel@surynek.net>                */
/*       http://users.fit.cvut.cz/surynek | <pavel.surynek@fit.cvut.cz>       */
/*                                                                            */
/*============================================================================*/
<<<<<<< HEAD
/* waxman_test.cpp / 0-107_nofutu                                             */
=======
/* waxman_test.cpp / 0-104_nofutu                                             */
>>>>>>> 14f14b75c8f1d7815a0fb722e1fd0403f95fb68c
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


sResult waxman_test_5(void)
{
    clock_t time_start, time_finish;

    time_start = clock();
    sPathEmbeddingModel path_embedding_Model;
    
    sBoolEncoder encoder;
    Glucose::Solver *solver = path_embedding_Model.setup_SATSolver();
	
    path_embedding_Model.m_physical_Network.generate_UndirectedWaxman(100, 0.5, 0.5, 50, 50);
    //path_embedding_Model.m_physical_Network.to_Screen();

    s_DirectedGraph *virtual_network;
    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->generate_DirectedWaxman(30, 0.5, 0.5, 50, 50);
    virtual_network->to_Screen();

    /*
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
    */    

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
    sDouble geographical_distance = 35.0;

    printf("Building model ...\n");
    path_embedding_Model.setup_LimitedLazyPathModel(&encoder, depth, geographical_distance);
    //path_embedding_Model.to_Screen();

    path_embedding_Model.build_LimitedLazyPathModel(&encoder, solver, depth);
    printf("Building model ... finished\n");

    sEmbeddingModel::Mappings_vector vertex_Embeddings;
    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;    

    if (path_embedding_Model.solveAll_LazyPathModel(&encoder, solver, vertex_Embeddings, path_Embeddings, depth))
    {
	path_embedding_Model.to_Screen_embedding(vertex_Embeddings, path_Embeddings);
	sDouble cost = path_embedding_Model.calc_EmbeddingCost(&encoder, solver, vertex_Embeddings, path_Embeddings, depth);

	printf("Cost: %.3f\n", cost);
	printf("Finally a correct embedding has been found !\n");		
    }
    else
    {
	printf("Embedding does NOT exist.\n");
    }    
    time_finish = clock();
    printf("Total CPU time: %.3f (seconds)\n", (time_finish - time_start) / (sDouble)CLOCKS_PER_SEC);
   
    return sRESULT_SUCCESS;
}


sResult waxman_test_5_geo_circ(void)
{
    clock_t time_start, time_finish;

    time_start = clock();
    sPathEmbeddingModel path_embedding_Model;
    
    sBoolEncoder encoder;
    Glucose::Solver *solver = path_embedding_Model.setup_SATSolver();
	
    path_embedding_Model.m_physical_Network.generate_UndirectedWaxman(100, 0.5, 0.5, 50, 50);
    //path_embedding_Model.m_physical_Network.to_Screen();

    s_DirectedGraph *virtual_network;
    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->generate_DirectedWaxman(40, 0.5, 0.5, 50, 50);
    virtual_network->to_Screen();

    /*
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
    */    

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
    sInt_32 depth = 3;
    sDouble geographical_distance = 15.0;

    sPathEmbeddingModel::GeoCircles_vector geo_Circles;    

    printf("Building model ...\n");
    path_embedding_Model.setup_LimitedLazyPathModel(&encoder, depth, geographical_distance);
    //path_embedding_Model.to_Screen();

    path_embedding_Model.build_LimitedLazyPathModel(&encoder, solver, depth, &geo_Circles);
    printf("Building model ... finished\n");

    sEmbeddingModel::Mappings_vector vertex_Embeddings;
    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;    

    if (path_embedding_Model.solveAll_LazyPathModel(&encoder, solver, vertex_Embeddings, path_Embeddings, depth, &geo_Circles))
    {
	path_embedding_Model.to_Screen_embedding(vertex_Embeddings, path_Embeddings);
	sDouble cost = path_embedding_Model.calc_EmbeddingCost(&encoder, solver, vertex_Embeddings, path_Embeddings, depth);

	printf("Cost: %.3f\n", cost);	
	printf("Finally a correct embedding has been found !\n");		
    }
    else
    {
	printf("Embedding does NOT exist.\n");
    }    
    time_finish = clock();
    printf("Total CPU time: %.3f (seconds)\n", (time_finish - time_start) / (sDouble)CLOCKS_PER_SEC);
   
    return sRESULT_SUCCESS;
}


sResult waxman_test_5_inc_geo_circ(void)
{
    clock_t time_start, time_finish;

    time_start = clock();
    sPathEmbeddingModel path_embedding_Model;
    
    sBoolEncoder encoder;
	
    path_embedding_Model.m_physical_Network.generate_UndirectedWaxman(100, 0.5, 0.5, 50, 50);
    //path_embedding_Model.m_physical_Network.to_Screen();

    s_DirectedGraph *virtual_network;
    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();

    virtual_network->generate_DirectedWaxman(40, 0.5, 0.5, 50, 50);
    virtual_network->to_Screen();

    sInt_32 max_depth = 10;
    sDouble geographical_distance = 15.0;

    sPathEmbeddingModel::GeoCircles_vector geo_Circles;    

    sEmbeddingModel::Mappings_vector vertex_Embeddings;
    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;

    sDouble cost;
    cost = path_embedding_Model.solveDepthIncreasing_LazyPathModel(&encoder,
								   vertex_Embeddings,
								   path_Embeddings,
								   geographical_distance,
								   max_depth,
								   &geo_Circles);
    
    if (cost >= 0)
    {
	path_embedding_Model.to_Screen_embedding(vertex_Embeddings, path_Embeddings);
	
	printf("Cost: %.3f\n", cost);
	printf("Finally a correct embedding has been found !\n");
    }
    else
    {
	printf("Embedding does NOT exist.\n");	
    }

    time_finish = clock();
    printf("Total CPU time: %.3f (seconds)\n", (time_finish - time_start) / (sDouble)CLOCKS_PER_SEC);
   
    return sRESULT_SUCCESS;
}


sResult waxman_test_5_tree(void)
{
    clock_t time_start, time_finish;

    time_start = clock();
    sPathEmbeddingModel path_embedding_Model;
    
    sBoolEncoder encoder;
    Glucose::Solver *solver = path_embedding_Model.setup_SATSolver();
	
    path_embedding_Model.m_physical_Network.generate_UndirectedWaxman(100, 0.5, 0.5, 50, 50);
//    path_embedding_Model.m_physical_Network.to_Screen();

    s_DirectedGraph *virtual_network;
    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();
    
/*  
    virtual_network->generate_DirectedWaxman(33, 0.5, 0.5, 50, 50);
    virtual_network->to_Screen();
*/
    
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
    sDouble geographical_distance = 15.0; // -1.0;

    printf("Building model ...\n");
    path_embedding_Model.setup_LimitedLazyTreeModel(&encoder, depth, geographical_distance);
    //path_embedding_Model.to_Screen();

    path_embedding_Model.build_LimitedLazyTreeModel(&encoder, solver, depth);
    printf("Building model ... finished\n");

    sEmbeddingModel::Mappings_vector vertex_Embeddings;
    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;    

    if (path_embedding_Model.solveAll_LazyTreeModel(&encoder, solver, vertex_Embeddings, path_Embeddings, depth))
    {
	path_embedding_Model.to_Screen_embedding(vertex_Embeddings, path_Embeddings);
	sDouble cost = path_embedding_Model.calc_EmbeddingCost(&encoder, solver, vertex_Embeddings, path_Embeddings, depth);

	printf("Cost: %.3f\n", cost);	
	printf("Finally a correct embedding has been found !\n");		
    }
    else
    {
	printf("Embedding does NOT exist.\n");
    }    
    time_finish = clock();
    printf("Total CPU time: %.3f (seconds)\n", (time_finish - time_start) / (sDouble)CLOCKS_PER_SEC);
   
    return sRESULT_SUCCESS;
}


sResult waxman_test_5_tree_geo_circ(void)
{
    clock_t time_start, time_finish;

    time_start = clock();
    sPathEmbeddingModel path_embedding_Model;
    
    sBoolEncoder encoder;
    Glucose::Solver *solver = path_embedding_Model.setup_SATSolver();
	
    path_embedding_Model.m_physical_Network.generate_UndirectedWaxman(100, 0.5, 0.5, 50, 50);
//    path_embedding_Model.m_physical_Network.to_Screen();

    s_DirectedGraph *virtual_network;
    path_embedding_Model.m_virtual_Networks.push_back(s_DirectedGraph());
    virtual_network = &path_embedding_Model.m_virtual_Networks.back();
    
  
    virtual_network->generate_DirectedWaxman(40, 0.5, 0.5, 50, 50);
    virtual_network->to_Screen();

/*    
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
*/

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
    sDouble geographical_distance = 15.0; // -1.0;

    sPathEmbeddingModel::GeoCircles_vector geo_Circles;        

    printf("Building model ...\n");
    path_embedding_Model.setup_LimitedLazyTreeModel(&encoder, depth, geographical_distance);
    //path_embedding_Model.to_Screen();

    path_embedding_Model.build_LimitedLazyTreeModel(&encoder, solver, depth, &geo_Circles);
    printf("Building model ... finished\n");

    sEmbeddingModel::Mappings_vector vertex_Embeddings;
    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;    

    if (path_embedding_Model.solveAll_LazyTreeModel(&encoder, solver, vertex_Embeddings, path_Embeddings, depth, &geo_Circles))
    {
	path_embedding_Model.to_Screen_embedding(vertex_Embeddings, path_Embeddings);
	sDouble cost = path_embedding_Model.calc_EmbeddingCost(&encoder, solver, vertex_Embeddings, path_Embeddings, depth);

	printf("Cost: %.3f\n", cost);	
	printf("Finally a correct embedding has been found !\n");		
    }
    else
    {
	printf("Embedding does NOT exist.\n");
    }    
    time_finish = clock();
    printf("Total CPU time: %.3f (seconds)\n", (time_finish - time_start) / (sDouble)CLOCKS_PER_SEC);
   
    return sRESULT_SUCCESS;
}


sResult waxman_test_5_graph(void)
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
    path_embedding_Model.setup_LimitedLazyGraphModel(&encoder, depth, geographical_distance);
    //path_embedding_Model.to_Screen();

    path_embedding_Model.build_LimitedLazyGraphModel(&encoder, solver, depth);
    printf("Building model ... finished\n");

    sEmbeddingModel::Mappings_vector vertex_Embeddings;
    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;    

    if (path_embedding_Model.solveAll_LazyGraphModel(&encoder, solver, vertex_Embeddings, path_Embeddings, depth))
    {
	path_embedding_Model.to_Screen_embedding(vertex_Embeddings, path_Embeddings);
	sDouble cost = path_embedding_Model.calc_EmbeddingCost(&encoder, solver, vertex_Embeddings, path_Embeddings, depth);

	printf("Cost: %.3f\n", cost);	
	printf("Finally a correct embedding has been found !\n");		
    }
    else
    {
	printf("Embedding does NOT exist.\n");
    }    
    time_finish = clock();
    printf("Total CPU time: %.3f (seconds)\n", (time_finish - time_start) / (sDouble)CLOCKS_PER_SEC);
   
    return sRESULT_SUCCESS;
}


sResult waxman_test_5_graph_geo_circ(void)
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

    virtual_network->generate_DirectedWaxman(30, 0.5, 0.5, 50, 50);
    virtual_network->to_Screen();    
/*
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
*/

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

    sPathEmbeddingModel::GeoCircles_vector geo_Circles;
    
    printf("Building model ...\n");
    path_embedding_Model.setup_LimitedLazyGraphModel(&encoder, depth, geographical_distance);
    //path_embedding_Model.to_Screen();

    path_embedding_Model.build_LimitedLazyGraphModel(&encoder, solver, depth, &geo_Circles);
    printf("Building model ... finished\n");

    sEmbeddingModel::Mappings_vector vertex_Embeddings;
    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;    

    if (path_embedding_Model.solveAll_LazyGraphModel(&encoder, solver, vertex_Embeddings, path_Embeddings, depth, &geo_Circles))
    {
	path_embedding_Model.to_Screen_embedding(vertex_Embeddings, path_Embeddings);
	sDouble cost = path_embedding_Model.calc_EmbeddingCost(&encoder, solver, vertex_Embeddings, path_Embeddings, depth);

	printf("Cost: %.3f\n", cost);	
	printf("Finally a correct embedding has been found !\n");		
    }
    else
    {
	printf("Embedding does NOT exist.\n");
    }    
    time_finish = clock();
    printf("Total CPU time: %.3f (seconds)\n", (time_finish - time_start) / (sDouble)CLOCKS_PER_SEC);
   
    return sRESULT_SUCCESS;
}


sResult waxman_test_6(void)
{
    clock_t time_start, time_finish;

    time_start = clock();
    sPathEmbeddingModel path_embedding_Model;

    s_DirectedGraph physical_network;
    physical_network.generate_UndirectedWaxman(100, 0.5, 0.5, 50, 50);
    physical_network.to_Screen();
    
    path_embedding_Model.setup_PhysicalNetwork_online(physical_network);
    	
    for (sInt_32 i = 0; i < 4; ++i)
    {
	s_DirectedGraph virtual_network;
	virtual_network.generate_DirectedWaxman(16, 0.5, 0.5, 50, 50);
	virtual_network.to_Screen();
	
	sInt_32 depth = 10;
	sDouble geographical_distance = 15.0;
	
	path_embedding_Model.setup_VirtualNetwork_online(virtual_network);
	
	sEmbeddingModel::Mappings_vector vertex_Embeddings;
	sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;    	
	
	if (path_embedding_Model.solve_LazyPathModel_online(depth, geographical_distance, vertex_Embeddings, path_Embeddings))
	{
	    path_embedding_Model.to_Screen_embedding(vertex_Embeddings, path_Embeddings);	
	    printf("Finally a correct embedding has been found !\n");		
	}
	else
	{
	    printf("Embedding does NOT exist.\n");
	}
	printf("--------------------------------\n");
    }
    time_finish = clock();

    printf("Total CPU time: %.3f (seconds)\n", (time_finish - time_start) / (sDouble)CLOCKS_PER_SEC);
   
    return sRESULT_SUCCESS;
}


sResult waxman_test_7(void)
{
    clock_t time_start, time_finish;

    time_start = clock();    
    for (sInt_32 step = 0; step < 4; ++step)
    {
	sPathEmbeddingModel path_embedding_Model;

	s_DirectedGraph physical_network;
	physical_network.generate_UndirectedWaxman(100, 0.5, 0.5, 50, 50);
	physical_network.to_Screen();

	path_embedding_Model.setup_PhysicalNetwork_online(physical_network);

	
	for (sInt_32 i = 0; i < 4; ++i)
	{
	    s_DirectedGraph virtual_network;
	    virtual_network.generate_DirectedWaxman(32, 0.5, 0.5, 50, 50);
	    virtual_network.to_Screen();
	    
	    sInt_32 depth = 10;
	    sDouble geographical_distance = 15.0;
	    
	    path_embedding_Model.setup_VirtualNetwork_online(virtual_network);
	    
	    sEmbeddingModel::Mappings_vector vertex_Embeddings;
	    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;    	
	    
	    if (path_embedding_Model.solve_LazyPathModel_online(depth, geographical_distance, vertex_Embeddings, path_Embeddings))
	    {
		path_embedding_Model.to_Screen_embedding(vertex_Embeddings, path_Embeddings);	
		printf("Finally a correct embedding has been found !\n");		
	    }
	    else
	    {
		printf("Embedding does NOT exist.\n");
	    }
	    printf("--------------------------------\n");
	}
	time_finish = clock();
    }
    printf("Total CPU time: %.3f (seconds)\n", (time_finish - time_start) / (sDouble)CLOCKS_PER_SEC);
   
    return sRESULT_SUCCESS;
}


sResult waxman_test_7_tree(void)
{
    clock_t time_start, time_finish;

    time_start = clock();    
    for (sInt_32 step = 0; step < 4; ++step)
    {
	sPathEmbeddingModel path_embedding_Model;

	s_DirectedGraph physical_network;
	physical_network.generate_UndirectedWaxman(100, 0.5, 0.5, 50, 50);
	physical_network.to_Screen();

	path_embedding_Model.setup_PhysicalNetwork_online(physical_network);

	
	for (sInt_32 i = 0; i < 4; ++i)
	{
	    s_DirectedGraph virtual_network;
	    virtual_network.generate_DirectedWaxman(32, 0.5, 0.5, 50, 50);
	    virtual_network.to_Screen();
	    
	    sInt_32 depth = 10;
	    sDouble geographical_distance = 15.0;
	    
	    path_embedding_Model.setup_VirtualNetwork_online(virtual_network);
	    
	    sEmbeddingModel::Mappings_vector vertex_Embeddings;
	    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;    	
	    
	    if (path_embedding_Model.solve_LazyTreeModel_online(depth, geographical_distance, vertex_Embeddings, path_Embeddings))
	    {
		path_embedding_Model.to_Screen_embedding(vertex_Embeddings, path_Embeddings);	
		printf("Finally a correct embedding has been found !\n");		
	    }
	    else
	    {
		printf("Embedding does NOT exist.\n");
	    }
	    printf("--------------------------------\n");
	}
	time_finish = clock();
    }
    printf("Total CPU time: %.3f (seconds)\n", (time_finish - time_start) / (sDouble)CLOCKS_PER_SEC);
   
    return sRESULT_SUCCESS;
}


sResult waxman_test_7_graph(void)
{
    clock_t time_start, time_finish;

    time_start = clock();    
    for (sInt_32 step = 0; step < 4; ++step)
    {
	sPathEmbeddingModel path_embedding_Model;

	s_DirectedGraph physical_network;
	physical_network.generate_UndirectedWaxman(100, 0.5, 0.5, 50, 50);
	physical_network.to_Screen();

	path_embedding_Model.setup_PhysicalNetwork_online(physical_network);

	
	for (sInt_32 i = 0; i < 4; ++i)
	{
	    s_DirectedGraph virtual_network;
	    virtual_network.generate_DirectedWaxman(32, 0.5, 0.5, 50, 50);
	    virtual_network.to_Screen();
	    
	    sInt_32 depth = 10;
	    sDouble geographical_distance = 15.0;
	    
	    path_embedding_Model.setup_VirtualNetwork_online(virtual_network);
	    
	    sEmbeddingModel::Mappings_vector vertex_Embeddings;
	    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;    	
	    
	    if (path_embedding_Model.solve_LazyGraphModel_online(depth, geographical_distance, vertex_Embeddings, path_Embeddings))
	    {
		path_embedding_Model.to_Screen_embedding(vertex_Embeddings, path_Embeddings);	
		printf("Finally a correct embedding has been found !\n");		
	    }
	    else
	    {
		printf("Embedding does NOT exist.\n");
	    }
	    printf("--------------------------------\n");
	}
	time_finish = clock();
    }
    printf("Total CPU time: %.3f (seconds)\n", (time_finish - time_start) / (sDouble)CLOCKS_PER_SEC);
   
    return sRESULT_SUCCESS;
}


sResult waxman_test_8(void)
{
    clock_t time_start, time_finish;

    time_start = clock();    
    for (sInt_32 step = 0; step < 1; ++step)
    {
	sPathEmbeddingModel path_embedding_Model;

	s_DirectedGraph physical_network;
	physical_network.generate_UndirectedWaxman(100, 0.5, 0.5, 50, 50);
	physical_network.to_Screen();

	path_embedding_Model.setup_PhysicalNetwork_online(physical_network);

	
	for (sInt_32 i = 0; i < 4; ++i)
	{
	    s_DirectedGraph virtual_network;
	    
	    virtual_network.add_Vertex(1.0);
	    virtual_network.add_Vertex(1.0);
	    virtual_network.add_Vertex(1.0);
	    virtual_network.add_Vertex(1.0);	    

	    /*
	    virtual_network.add_Arrow(1, 0, 1.0);
	    virtual_network.add_Arrow(3, 0, 1.0);
	    virtual_network.add_Arrow(1, 2, 1.0);
	    virtual_network.add_Arrow(3, 2, 1.0);	    
	    */
	    virtual_network.add_Arrow(0, 1, 1.0);
	    virtual_network.add_Arrow(1, 2, 1.0);
	    virtual_network.add_Arrow(2, 3, 1.0);
	    virtual_network.add_Arrow(3, 0, 1.0);	    	    

	    virtual_network.to_Screen();
	    
	    sInt_32 depth = 10;
	    sDouble geographical_distance = 15.0;

	    path_embedding_Model.setup_VirtualNetwork_online(virtual_network);

	    sEmbeddingModel::Mappings_vector vertex_Embeddings;
	    sPathEmbeddingModel::NetworkPathMappings_vector path_Embeddings;
	    
	    if (path_embedding_Model.solve_LazyPathModel_online(depth, geographical_distance, vertex_Embeddings, path_Embeddings))
	    {
		path_embedding_Model.to_Screen_embedding(vertex_Embeddings, path_Embeddings);	
		printf("Finally a correct embedding has been found !\n");		
	    }
	    else
	    {
		printf("Embedding does NOT exist.\n");
	    }
	    printf("--------------------------------\n");
	}
	time_finish = clock();
    }
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
/*
    if (sFAILED(result = waxman_test_4()))
    {
        printf("Test waxman 4 failed (error:%d).\n", result);
        return result;
    } 
*/
/*
    if (sFAILED(result = waxman_test_5()))
    {
	printf("Test waxman 5 failed (error:%d).\n", result);
	return result;
    }
*/
<<<<<<< HEAD
/*
=======

>>>>>>> 14f14b75c8f1d7815a0fb722e1fd0403f95fb68c
    if (sFAILED(result = waxman_test_5_geo_circ()))
    {
	printf("Test waxman 5 failed (error:%d).\n", result);
	return result;
    }    
<<<<<<< HEAD
*/

    if (sFAILED(result = waxman_test_5_inc_geo_circ()))
    {
	printf("Test waxman 5 failed (error:%d).\n", result);
	return result;
    }
=======
>>>>>>> 14f14b75c8f1d7815a0fb722e1fd0403f95fb68c

/*
    if (sFAILED(result = waxman_test_5_tree()))
    {
	printf("Test waxman 5 failed (error:%d).\n", result);
	return result;
    }
*/
/*
    if (sFAILED(result = waxman_test_5_tree_geo_circ()))
    {
	printf("Test waxman 5 failed (error:%d).\n", result);
	return result;
    }
*/    
/*
    if (sFAILED(result = waxman_test_5_graph()))
    {
	printf("Test waxman 5 failed (error:%d).\n", result);
	return result;
    }    
*/
/*
    if (sFAILED(result = waxman_test_5_graph_geo_circ()))
    {
	printf("Test waxman 5 failed (error:%d).\n", result);
	return result;
    }
*/
/*
    if (sFAILED(result = waxman_test_6()))
    {
	printf("Test waxman 6 failed (error:%d).\n", result);
	return result;
    }        
*/
/*
    if (sFAILED(result = waxman_test_7()))
    {
	printf("Test waxman 7 failed (error:%d).\n", result);
	return result;
    }
*/
/*
    if (sFAILED(result = waxman_test_7_tree()))
    {
	printf("Test waxman 7 failed (error:%d).\n", result);
	return result;
    } 
*/
/*
    if (sFAILED(result = waxman_test_7_graph()))
    {
	printf("Test waxman 7 failed (error:%d).\n", result);
	return result;
    } 
*/
/*    
    if (sFAILED(result = waxman_test_8()))
    {
	printf("Test waxman 8 failed (error:%d).\n", result);
	return result;
    }                
*/
    return sRESULT_SUCCESS;
}
    
