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
const std::string OUTPUT_FILE = "results.txt"; 

struct Data {
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


    Data(int numJobs_arg, int numTools_arg, int magCapacity_arg, std::vector<std::vector<int> >& jobsToolsMatrix_arg)
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
    }
    
};

unsigned int(*func_to_test)(Data&, std::vector<int>&);
std::string func_to_test_name;


unsigned int KTNS(Data& data, std::vector<int>& chromosome) {
    int jump = -1;

    for (int i = (data.numJobs - 1); i >= 0; i--) {
        for (unsigned int j = 0; j < data.numTools; j++) {
            if (i != jump && data.jobsToolsMatrix[chromosome[i]][j] == 1) {
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


unsigned int IGA_bitwise_64(Data& data, std::vector<int>& chromosome) {
    unsigned long long end_tools, intersect;

    for (int i = 0; i < data.numJobs; i++) {
        data.cromosome_bytes[i] = data.jobnum_to_bytes[chromosome[i]];
        data.cromosome_free[i] = data.jobnum_to_free[chromosome[i]];
    }

    int dist = data.start_dist;
    for (int i = 0; i < data.numJobs - 1; i++) {
        dist -= data.jobs_pairs_intersect[chromosome[i]][chromosome[i + 1]];
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


unsigned int IGA_bitwise_128(Data& data, std::vector<int>& chromosome) {
    int min_free, end, start, tubes_count, j;
    std::bitset<BITSET_SIZE> end_tools, intersect;
    for (int i = 0; i < data.numJobs; i++) {
        data.cromosome_bytes_bitset[i] = data.jobnum_to_bytes_bitset[chromosome[i]];
        data.cromosome_free[i] = data.jobnum_to_free[chromosome[i]];
    }

    int dist = data.start_dist;
    for (int i = 0; i < data.numJobs - 1; i++) {
        dist -= data.jobs_pairs_intersect[chromosome[i]][chromosome[i + 1]];
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

unsigned int IGA_bit(Data& data, std::vector<int>& chromosome) {
    assert(data.numTools <= 128);
    unsigned int result;
    
    if (data.numTools <= 64) result =  IGA_bitwise_64(data, chromosome);
    else  result = IGA_bitwise_128(data, chromosome);

    return result;
}

unsigned int IGA(Data& data, std::vector<int>& chromosome) {
    int i,end, passive_intervals_count = 0, last_full = -1;
    data.last_seen_tool.assign(data.numTools, -2);

    for (end = 0; end < data.numJobs; end++) {
        for (int tool : data.jobnum_to_tools_list[chromosome[end]]) {
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
        data.filled_slots[end] = data.job_capacity[chromosome[end]];
        if (data.filled_slots[end] == data.magCapacity) { last_full = end; }
    }
    return data.start_dist - passive_intervals_count;
}



unsigned int IGAfull(Data& data, std::vector<int>& chromosome) {
    int i, j, end, passive_intervals_count = 0, last_full = -1, first_last_full = -2;;
    data.last_seen_tool.assign(data.numTools, -2);
    

    for (end = 0; end < data.numJobs; end++) {
        data.tool_in_i_not_in_prev[end].clear();
        data.MAGAZINE[end].clear();
        for (int tool : data.jobnum_to_tools_list[chromosome[end]]) {
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
        data.filled_slots[end] = data.job_capacity[chromosome[end]];
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


double algorithm_time(std::string dataset_path) {
    int n = -1, m = -1, C = -1;
    std::ifstream fin(dataset_path);

    fin >> n >> m >> C;
    assert(n > 0);
    assert(m > 0);
    assert(C > 0);
    std::vector<std::vector<int> > jobsToolsMatrix(n, std::vector<int>(m, -1));
    int a;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            fin >> jobsToolsMatrix[j][i];
        }
    }
    fin.close();

    Data data(n, m, C, jobsToolsMatrix);


    int n_premuts = (n <= 40) ? 100000 : 200000;
    std::vector<std::vector<int> > quest(n_premuts);
    std::vector<int> rand_perm(n); 
    for(int i=0;i<n;i++) rand_perm[i] = i;
    for(int i=0;i<n_premuts;i++){
        quest[i] = rand_perm;
        std::random_shuffle(rand_perm.begin(), rand_perm.end());
    }

    std::cout << "alg=" << func_to_test_name << ", n=" << n << ", m=" << m << ", C=" << C << ", file=" << dataset_path << ".\n";
    int result;
    clock_t start = clock();
    for (int i = 0; i < n_premuts; i++) {
        result = func_to_test(data, quest[i]);
    }
    clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    return seconds;
}





double dataset_time_catanzaro(std::string folde_name, std::string dataset) {
    std::string dir = "./";
    dir += folde_name + "/";
    std::string ABCD = std::string() + dataset[0];
    std::string capacity_banchmark_index = std::string() + dataset[1] ;


    double summary_time = 0;
    for (int instance_num = 1; instance_num <= 10; instance_num++) {
        std::string dataset_path = dir + "Tabela" + capacity_banchmark_index + "/dat" + ABCD + std::to_string(instance_num);
        std::string prem_file_path = dir + "Tabela" + capacity_banchmark_index + "/permuts" + ABCD;
        summary_time += algorithm_time(dataset_path);
    }
    return summary_time;
}

double dataset_time_mecler(std::string folde_name, std::string dataset) {
    std::string dir = "./";
    dir += folde_name + "/";
    std::string ABCD = std::string() + dataset[0]  + dataset[1];
    std::string capacity_banchmark_index = std::string() + dataset[3];


    double summary_time = 0;
    for (int instance_num = 1; instance_num <= 5; instance_num++) {
        std::string dataset_path = dir + "Tabela" + capacity_banchmark_index + "/" + ABCD + "00" + std::to_string(instance_num) + ".txt";
        std::string prem_file_path = dir + "Tabela" + capacity_banchmark_index + "/permuts_2_" + ABCD;
        summary_time += algorithm_time(dataset_path);
    }
    return summary_time;
}


void run_tests() {
    
    std::unordered_map<std::string, double> time_dict;
    
    std::vector<std::string> datasets1;
    datasets1.push_back("A1");
    datasets1.push_back("A2");
    datasets1.push_back("A3");
    datasets1.push_back("A4");
    datasets1.push_back("B1");
    datasets1.push_back("B2");
    datasets1.push_back("B3");
    datasets1.push_back("B4");
    datasets1.push_back("C1");
    datasets1.push_back("C2");
    datasets1.push_back("C3");
    datasets1.push_back("C4");
    datasets1.push_back("D1");
    datasets1.push_back("D2");
    datasets1.push_back("D3");
    datasets1.push_back("D4");

    std::ofstream output_file;
    output_file.open("results.txt", std::ios::app);
    output_file << "algorithm: " << func_to_test_name << "\n";
    output_file.close();

    for (std::string dataset : datasets1) {
        double summary_time = dataset_time_catanzaro("Catanzaro", dataset);
        time_dict[dataset] = summary_time;

        std::ofstream output_file;
        output_file.open("results.txt", std::ios::app);
        output_file << "'" << dataset << "': " << time_dict[dataset] << ",\n";
        output_file.close();
    }
    std::vector<std::string> datasets2;
    datasets2.push_back("F1.1");
    datasets2.push_back("F1.2");
    datasets2.push_back("F1.3");
    datasets2.push_back("F1.4");
    datasets2.push_back("F2.1");
    datasets2.push_back("F2.2");
    datasets2.push_back("F2.3");
    datasets2.push_back("F2.4");
    datasets2.push_back("F3.1");
    datasets2.push_back("F3.2");
    datasets2.push_back("F3.3");
    datasets2.push_back("F3.4");

    for (std::string dataset : datasets2) {
        double summary_time = dataset_time_mecler("Mecler", dataset);
        time_dict[dataset] = summary_time;
        
        std::ofstream output_file;
        output_file.open("results.txt", std::ios::app);
        output_file << "'" << dataset << "': " << time_dict[dataset] << ",\n";
        output_file.close();
    }
    
}


int main() {
    srand(time(NULL));
    std::cout << std::fixed << std::setprecision(5);
    std::ofstream output_file; output_file.open(OUTPUT_FILE, std::ofstream::out | std::ofstream::trunc); output_file.close();


    func_to_test_name = "IGA_bit";
    func_to_test = IGA_bit;
    run_tests();

    func_to_test_name = "IGA";
    func_to_test = IGA;
    run_tests();

    func_to_test_name = "IGAfull";
    func_to_test = IGAfull;
    run_tests();

    func_to_test_name = "KTNS";
    func_to_test = KTNS;
    run_tests();


    return 0;
}



