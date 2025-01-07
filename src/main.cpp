#include <iostream>
#include <fstream>
#include <vector>
#include <string> 
#include <stdio.h> 
#include <time.h> 
#include <unordered_map>
#include <unordered_set>
#include <bitset>
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <cassert>
#include <iomanip>


const int BITSET_SIZE = 128;
const std::string OUTPUT_FILE = ".//results/results.txt"; 

struct TRP_Data {
    /*
        Container for a TRP instance data.
    */
    std::vector<std::vector<int> > jobsToolsMatrix;
    std::vector<std::vector<int> > L;
    std::vector<std::vector<int> > loadedMatrix;
    int numJobs, numTools, magCapacity;
    std::vector<int> used;
    std::vector<int> W_n;

    int start_dist;
    std::vector<int> filled_slots;
    std::vector<int> job_capacity;
    std::vector<int> cromosome_free;
    
    std::vector<unsigned long long> cromosome_bytes;
    std::vector<std::bitset<BITSET_SIZE> > cromosome_bytes_bitset;
    std::vector<int> jobnum_to_free;
    std::vector<unsigned long long> jobnum_to_bytes;
    std::vector<std::bitset<BITSET_SIZE> > jobnum_to_bytes_bitset;
    std::vector<std::vector<int> > jobs_pairs_intersect;

    std::vector<std::vector<int> > jobnum_to_tools_list;
    std::vector<int> last_seen_tool;
    std::vector<int> usedToFullMag;
    std::vector<std::vector<int> > MAGAZINE;
    std::vector<std::vector<int> > H_1_MAG;
    std::vector<std::vector<int> > tool_in_i_not_in_prev;
    std::vector<std::unordered_set<int> > MAG_SETS;
    std::vector<std::unordered_set<int> > T_SETS;
    std::vector<int> empty_vec;
    std::vector<std::vector<int> > tool_in_i_not_in_prev_start;
    std::vector<int> job_sequence;


    TRP_Data(int numJobs_arg, int numTools_arg, int magCapacity_arg, std::vector<std::vector<int> >& jobsToolsMatrix_arg)
        :numJobs(numJobs_arg),
        numTools(numTools_arg),
        magCapacity(magCapacity_arg),
        jobsToolsMatrix(jobsToolsMatrix_arg) {

        used.assign(numTools, 0);
        L.assign(numTools, std::vector<int>(numJobs, -1));
        W_n.assign(numTools, -1);
        loadedMatrix.assign(numJobs, std::vector<int>(numTools, -1));

        jobnum_to_bytes.assign(numJobs, 0LL);
        jobnum_to_bytes_bitset.assign(numJobs, 0LL);
        for (int i = 0; i < numJobs; i++) {
            std::string str = "";
            for (auto v : jobsToolsMatrix[i]) { str += std::to_string(v); }
	        std::reverse(str.begin(), str.end());
            jobnum_to_bytes_bitset[i] = std::bitset<BITSET_SIZE>(str);
            jobnum_to_bytes[i] = std::bitset<64>(str).to_ullong();
        }
        cromosome_bytes.assign(numJobs, 0LL);
        cromosome_bytes_bitset.assign(numJobs, 0LL);
        start_dist = magCapacity * (numJobs - 1);
        jobnum_to_free.assign(numJobs, 0);
	    job_capacity.assign(numJobs, 0);
        for (int i = 0; i < numJobs; i++) {
	    job_capacity[i] = (int)count(jobsToolsMatrix[i].begin(), jobsToolsMatrix[i].end(), 1);
            jobnum_to_free[i] = magCapacity - (int)count(jobsToolsMatrix[i].begin(), jobsToolsMatrix[i].end(), 1);
            start_dist -= jobnum_to_free[i];
        }
        cromosome_free.assign(numJobs, 0);
	    filled_slots.assign(numJobs, 0);
	
        jobs_pairs_intersect.assign(numJobs, std::vector<int>(numJobs, 0));
        for (int i = 0; i < numJobs; i++) {
            for (int j = 0; j < numJobs; j++) {
                jobs_pairs_intersect[i][j] = (jobnum_to_bytes_bitset[i] & jobnum_to_bytes_bitset[j]).count();
            }
        }

        jobnum_to_tools_list.assign(numJobs, std::vector<int>(0));
        for (int i = 0; i < numJobs; i++) {
            for (int j = 0; j < numTools; j++) {
                if (jobsToolsMatrix[i][j] == 1) {
                    jobnum_to_tools_list[i].push_back(j);
                }

            }
        }
        last_seen_tool.assign(numTools, -1);
        usedToFullMag.assign(numTools, 0);
        MAG_SETS.assign(numTools, std::unordered_set<int>());
        T_SETS.assign(numTools, std::unordered_set<int>());
        for (int i = 0; i < numJobs; i++) {
            for (auto& tool : jobnum_to_tools_list[i]) {
                T_SETS[i].insert(tool);
            }
        }
        empty_vec.assign(0,0);
        tool_in_i_not_in_prev_start.assign(numJobs, std::vector<int>(0));
        tool_in_i_not_in_prev.assign(numJobs, std::vector<int>(0));
        MAGAZINE.assign(numJobs, std::vector<int>(0));
        H_1_MAG.assign(numJobs, std::vector<int>(0));
        usedToFullMag.assign(numTools, 0);
        job_sequence.assign(numJobs, 0LL);
    }
    
};

unsigned int(*func_to_test)(TRP_Data&, std::vector<int>&);
std::string func_to_test_name;


unsigned int KTNS(TRP_Data& data) {
    /*
        Keep Tool Needed Soonest algortihm.
    */
    int jump = -1;

    for (int i = (data.numJobs - 1); i >= 0; i--) {
        for (unsigned int j = 0; j < data.numTools; j++) {
            if (i != jump && data.jobsToolsMatrix[data.job_sequence[i]][j] == 1) {
                data.L[j][i] = (unsigned int)i;
            }
            else if (i < (int)data.numJobs - 1) {
                data.L[j][i] = data.L[j][i + 1];
            }
            else {
                data.L[j][i] = data.numJobs;
            }
            data.used[j] = false;
        }
    }



    unsigned int switches = 0;
    unsigned int capacity = 0;
    unsigned int tool = 0;
    double minVal;

    for (unsigned int i = 0; i < data.numTools; i++) {
        if (data.L[i][0] == 0) {
            data.W_n[i] = 1;
            data.used[i] = true;
            capacity++;
        }
        else {
            data.W_n[i] = 0;
        }
    }

    while (capacity < data.magCapacity) {
        minVal = std::numeric_limits<double>::infinity();
        for (unsigned int i = 0; i < data.numTools; i++) {
            if (!data.used[i] && (data.L[i][0] < minVal)) {
                tool = i;
                minVal = data.L[i][0];
            }
        }
        data.used[tool] = true;
        data.W_n[tool] = 1;
        capacity++;
    }

    data.loadedMatrix[0] = data.W_n;

    unsigned int maxVal;
    for (unsigned int n = 1; n < data.numJobs; n++) {
        for (unsigned int i = 0; i < data.numTools; i++) {
            if (data.W_n[i] != 1 && data.L[i][n] == n) {
                data.W_n[i] = 1;
                capacity++;
            }
        }
        while (capacity > data.magCapacity) {
            maxVal = n;
            for (unsigned int i = 0; i < data.numTools; i++) {
                if (data.W_n[i] == 1 && data.L[i][n] > maxVal) {
                    tool = i;
                    maxVal = data.L[i][n];
                }
            }
            data.W_n[tool] = 0;
            capacity--;
            switches++;
        }
        data.loadedMatrix[n] = data.W_n;
    }
    return switches;
}


unsigned int IGA_bitwise_64(TRP_Data& data) {
     /*
        Bitwise (64) implementation of IGA.
    */
    unsigned long long end_tools, intersect;

    for (int i = 0; i < data.numJobs; i++) {
        data.cromosome_bytes[i] = data.jobnum_to_bytes[data.job_sequence[i]];
        data.cromosome_free[i] = data.jobnum_to_free[data.job_sequence[i]];
    }

    int dist = data.start_dist;
    for (int i = 0; i < data.numJobs - 1; i++) {
        dist -= data.jobs_pairs_intersect[data.job_sequence[i]][data.job_sequence[i + 1]];
    }
    int fullmag = -1;

    for (int end = 1; end < data.numJobs; end++) {
        int end_minus_1 = end - 1;
        if (data.cromosome_free[end_minus_1] == 0) { fullmag = end_minus_1; }
        end_tools = data.cromosome_bytes[end] & (~data.cromosome_bytes[end_minus_1]);
        int min_free = 99999;
        for (int start = end - 2; start >= 0; start--) {
            if (fullmag > start || end_tools == 0LL) { break; }
            if (min_free > data.cromosome_free[start + 1]) { min_free = data.cromosome_free[start + 1]; }
            intersect = data.cromosome_bytes[start] & end_tools;
            if (intersect > 0) {
                end_tools = end_tools & (~intersect);
                int tubes_count = (int)__builtin_popcountll(intersect);
                if (tubes_count > min_free) { tubes_count = min_free; }
                if (tubes_count > 0) {
                    for (int j = start + 1; j < end; j++) {
                        data.cromosome_free[j] -= tubes_count;
                        if (data.cromosome_free[j] == 0) { fullmag = j; }
                    }
                    dist -= tubes_count;
                    min_free -= tubes_count;
                }
            }
        }
    }

    return dist;
}


unsigned int IGA_bitwise_128(TRP_Data& data) {
     /*
        Bitwise (128) implementation of IGA.
    */
    int min_free, end, start, tubes_count, j;
    std::bitset<BITSET_SIZE> end_tools, intersect;
    for (int i = 0; i < data.numJobs; i++) {
        data.cromosome_bytes_bitset[i] = data.jobnum_to_bytes_bitset[data.job_sequence[i]];
        data.cromosome_free[i] = data.jobnum_to_free[data.job_sequence[i]];
    }

    int dist = data.start_dist;
    for (int i = 0; i < data.numJobs - 1; i++) {
        dist -= data.jobs_pairs_intersect[data.job_sequence[i]][data.job_sequence[i + 1]];
    }
    int fullmag = -1;

    for (end = 1; end < data.numJobs; end++) {
        if (data.cromosome_free[end - 1] == 0) { fullmag = end - 1; }
        end_tools = data.cromosome_bytes_bitset[end] & (~data.cromosome_bytes_bitset[end - 1]);
        min_free = 99999;
        for (start = end - 2; start >= 0; start--) {
            if (fullmag > start || end_tools.none()) { break; }
            if (min_free > data.cromosome_free[start + 1]) { min_free = data.cromosome_free[start + 1]; }
            intersect = data.cromosome_bytes_bitset[start] & end_tools;
            if (!intersect.none()) {
                end_tools = end_tools & (~intersect);
                tubes_count = intersect.count();
                if (tubes_count > min_free) { tubes_count = min_free; }
                if (tubes_count > 0) {
                    for (j = start + 1; j < end; j++) {
                        data.cromosome_free[j] -= tubes_count;
                        if (data.cromosome_free[j] == 0) { fullmag = j; }
                    }
                    dist -= tubes_count;
                    min_free -= tubes_count;
                }
            }
        }
    }

    return dist;
}

unsigned int IGA_bit(TRP_Data& data) {
    assert(data.numTools <= 128);
    unsigned int result;
    
    if (data.numTools <= 64) result =  IGA_bitwise_64(data);
    else  result = IGA_bitwise_128(data);

    return result;
}

unsigned int IGA(TRP_Data& data) {
    /*
        Main implementation of IGA.
    */
    int i,end, passive_intervals_count = 0, last_full = -1;
    data.last_seen_tool.assign(data.numTools, -2);

    for (end = 0; end < data.numJobs; end++) {
        for (int tool : data.jobnum_to_tools_list[data.job_sequence[end]]) {
            if (last_full <= data.last_seen_tool[tool]) {
                for (i = end - 1; i > data.last_seen_tool[tool]; i--) {
                    if ((++data.filled_slots[i]) == data.magCapacity) {
                        last_full = i;
                        break;
                    }
                }
                passive_intervals_count++;
            }
            data.last_seen_tool[tool] = end;
        }
        data.filled_slots[end] = data.job_capacity[data.job_sequence[end]];
        if (data.filled_slots[end] == data.magCapacity) { last_full = end; }
    }
    return data.start_dist - passive_intervals_count;
}



unsigned int IGAfull(TRP_Data& data) {
    /*
        IGAfull algorithm consists of IGA and ToFullMag procedures as subroutines.
    */
    int i, j, end, passive_intervals_count = 0, last_full = -1, first_last_full = -2;;
    data.last_seen_tool.assign(data.numTools, -2);
    

    for (end = 0; end < data.numJobs; end++) {
        data.tool_in_i_not_in_prev[end].clear();
        data.MAGAZINE[end].clear();
        for (int tool : data.jobnum_to_tools_list[data.job_sequence[end]]) {
            if (last_full <= data.last_seen_tool[tool]) {
                for (i = data.last_seen_tool[tool] + 1; i < end; i++) {
                    data.MAGAZINE[i].push_back(tool);
                    if ((++data.filled_slots[i]) == data.magCapacity) { last_full = i; }
                }
                passive_intervals_count++;
            }
            else {
                data.tool_in_i_not_in_prev[end].push_back(tool);
            }
            data.MAGAZINE[end].push_back(tool);
            data.last_seen_tool[tool] = end;
        }
        data.filled_slots[end] = data.job_capacity[data.job_sequence[end]];
        if (data.filled_slots[end] == data.magCapacity) { last_full = end; }
    }


    
    if (last_full < 0) { 
        last_full = 0; 
        first_last_full = -1;
    }
    for (i = last_full; i < data.numJobs - 1; i++) {
        j = i + 1;
        if (data.filled_slots[j] < data.magCapacity) {
            for (int tool : data.MAGAZINE[j]) { data.usedToFullMag[tool] = 1; }
            for (int tool : data.MAGAZINE[i]) {
                if (data.usedToFullMag[tool] == 0) {
                    data.MAGAZINE[j].push_back(tool);
                    if ((++data.filled_slots[j]) == data.magCapacity) { 
                        if (first_last_full == -1) { first_last_full = j; }
                        break; 
                    }
                }
            }
            for (int tool : data.MAGAZINE[j]) { data.usedToFullMag[tool] = 0; }
        }
    }
    

    if (first_last_full!=-2) {
        last_full = first_last_full;
    }
    for (i = last_full; i > 0; i--) {
        j = i - 1;
        data.H_1_MAG[j].clear();
        if (data.filled_slots[j] < data.magCapacity) {
            for (int tool : data.tool_in_i_not_in_prev[i]) {
                data.MAGAZINE[j].push_back(tool);
                data.H_1_MAG[j].push_back(tool);
                if ((++data.filled_slots[j]) == data.magCapacity) { break; }
            }
            if (data.filled_slots[j] == data.magCapacity) { continue;  }
            for (int tool : data.MAGAZINE[j]) { data.usedToFullMag[tool] = 1; }
            for (int tool : data.H_1_MAG[i]) {
                if (data.usedToFullMag[tool] == 0) {
                    data.MAGAZINE[j].push_back(tool);
                    data.H_1_MAG[j].push_back(tool);
                    if ((++data.filled_slots[j]) == data.magCapacity) { break; }

                }
            }
            for (int tool : data.MAGAZINE[j]) { data.usedToFullMag[tool] = 0; }
        }
    }

    return data.start_dist - passive_intervals_count;
}




struct Experiments
{
    /*
        Run computational experiments for the given algorithm, i.e. enumerate problem instances,
        read data from files, execute algorithm, measure the time spent on calculations,
        write cumputational results to file results.txt.
    */
    unsigned int (*alg_for_test)(TRP_Data &);
    std::string alg_for_test_name;

    Experiments(unsigned int (*alg_for_test_arg)(TRP_Data &),
                std::string alg_for_test_name_arg)
    {
        this->alg_for_test = alg_for_test_arg;
        this->alg_for_test_name = alg_for_test_name_arg;
    }
    double instance_time(std::string TRP_instance_path, int NUMBER_OF_JOB_SEQUENCES)
    {
        /*
            For the given TRP instance runs an algorithm for 
            generated random job sequences and returns total time spent.
        */

        // read TRP data from file
        int n = -1, m = -1, C = -1;
        std::ifstream fin(TRP_instance_path);
        fin >> n >> m >> C;
        assert(n > 0);
        assert(m > 0);
        assert(C > 0);
        std::vector<std::vector<int>> jobsToolsMatrix(n, std::vector<int>(m, -1));
        for (int tool = 0; tool < m; tool++)
            for (int job = 0; job < n; job++)
                fin >> jobsToolsMatrix[job][tool];
        fin.close();
        TRP_Data data(n, m, C, jobsToolsMatrix);

        std::vector<std::vector<int>> quest(NUMBER_OF_JOB_SEQUENCES);
        std::vector<int> rand_job_sequence(n);
        for (int i = 0; i < n; i++)
            rand_job_sequence[i] = i;
        for (int i = 0; i < NUMBER_OF_JOB_SEQUENCES; i++)
        {
            quest[i] = rand_job_sequence;
            std::random_shuffle(rand_job_sequence.begin(), rand_job_sequence.end());
        }

        // execute the algorithm for each job sequence
        std::cout << "alg=" << alg_for_test_name << ", n=" << n << ", m=" << m << ", C=" << C << ", file=" << TRP_instance_path << "." << std::endl;
        clock_t start = clock();
        for (int i = 0; i < NUMBER_OF_JOB_SEQUENCES; i++)
        {
            data.job_sequence = quest[i];
            alg_for_test(data);
        }
        clock_t end = clock();
        double seconds = (double)(end - start) / CLOCKS_PER_SEC;
        return seconds;
    }

    double dataset_time(std::string dataset)
    {
        /*
            Triggers 10^6 tests. 
            (100000 sequences) x (10 instances) =  10^6 for Catanzaro datasets.
            (200000 sequences) x (5 instances) =  10^6 for Mecler datasets.
        */
       double total_time = 0;

        if(dataset[0] == 'F')
        {
            int NUMBER_OF_JOB_SEQUENCES = 200000;
            std::string ABCD = std::string() + dataset[0]+ dataset[1];
            std::string capacity_banchmark_index = std::string() + dataset[3];
            for (int instance_num = 1; instance_num <= 5; instance_num++)
            {
                std::string TRP_instance_path = "./data/Mecler/Tabela" + capacity_banchmark_index + "/" + ABCD + "00" + std::to_string(instance_num) + ".txt";
                total_time += instance_time(TRP_instance_path, NUMBER_OF_JOB_SEQUENCES);
            }
        } 
        else
        {
            int NUMBER_OF_JOB_SEQUENCES = 100000;
            std::string ABCD = std::string() + dataset[0];
            std::string capacity_banchmark_index = std::string() + dataset[1];
            for (int instance_num = 1; instance_num <= 10; instance_num++)
            {
                std::string TRP_instance_path = "./data/Catanzaro/Tabela" + capacity_banchmark_index + "/dat" + ABCD + std::to_string(instance_num);
                total_time += instance_time(TRP_instance_path, NUMBER_OF_JOB_SEQUENCES);
            }
        }

        return total_time;
    }

    void run_tests()
    {
        /*
            Triggers running tests and writes test results to a file results.txt.
        */

        // list of datasets
        std::vector<std::string> datasets;
        datasets.push_back("A1");
        datasets.push_back("A2");
        // datasets.push_back("A3");
        // datasets.push_back("A4");
        // datasets.push_back("B1");
        // datasets.push_back("B2");
        // datasets.push_back("B3");
        // datasets.push_back("B4");
        // datasets.push_back("C1");
        // datasets.push_back("C2");
        // datasets.push_back("C3");
        // datasets.push_back("C4");
        // datasets.push_back("D1");
        // datasets.push_back("D2");
        // datasets.push_back("D3");
        // datasets.push_back("D4");

        // datasets.push_back("F1.1");
        // datasets.push_back("F1.2");
        // datasets.push_back("F1.3");
        // datasets.push_back("F1.4");
        // datasets.push_back("F2.1");
        // datasets.push_back("F2.2");
        // datasets.push_back("F2.3");
        // datasets.push_back("F2.4");
        // datasets.push_back("F3.1");
        // datasets.push_back("F3.2");
        // datasets.push_back("F3.3");
        // datasets.push_back("F3.4");

        std::ofstream output_file;
        for (std::string dataset_name : datasets)
        {
            // trigger tests
            double summary_time = dataset_time(dataset_name);
            std::cout << "\"" << dataset_name << "\": " << summary_time << "," << std::endl;

            // write results
            output_file.open(OUTPUT_FILE, std::ios::app);
            output_file << "{\"algorithm\": \"" 
                        << alg_for_test_name 
                        << "\", \"" << dataset_name << "\": " << summary_time << "}" << std::endl;
            output_file.close();
        }
    }
};

int main()
{
    srand(time(NULL));
    std::cout << std::fixed << std::setprecision(3);
    std::ofstream output_file;
    output_file.open(OUTPUT_FILE, std::ofstream::out | std::ofstream::trunc);
    output_file.close();

    Experiments test_IGAfull(IGAfull, "IGA-full");
    test_IGAfull.run_tests();

    Experiments test_IGA_bit(IGA_bit, "IGA-bit");
    test_IGA_bit.run_tests();

    Experiments test_IGA(IGA, "IGA");
    test_IGA.run_tests();

    Experiments test_KTNS(KTNS, "KTNS");
    test_KTNS.run_tests();

    return 0;
}