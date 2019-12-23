use rand::Rng;

pub fn DARNp_generator(nodes: i32,
    mem_leng: i32,
    mem_strength: f64,
    avg_prob: f64,
    time_steps: i32,
    wait_time: i32) -> Vec<Vec<i8>> {

    let mut rng = rand::thread_rng();
    let mut check_rand_1: f64;         //For checking rng outputs.
    let mut check_rand_2: f64;         //For checking rng outputs.
    let mut rand_mem: i32;             //For checking past links to use.

    let links: i32 = nodes * (nodes-1)/2;

    let mut edge_series: Vec<Vec<i8>> = vec![vec![0;(time_steps+wait_time+mem_leng) as usize]; links as usize];

    for l in 0..links {
        for t in 0..mem_leng {
            check_rand_1 = rng.gen();
            if check_rand_1 < avg_prob {
                edge_series[l as usize][t as usize] = 1;
            }
        }
    }
    for l in 0..links {
        for t in mem_leng..(time_steps+wait_time) {
            check_rand_1 = rng.gen();
            if check_rand_1 < mem_strength {
                rand_mem = rng.gen_range(1,mem_leng+1);
                edge_series[l as usize][t as usize] = edge_series[l as usize][(t-rand_mem) as usize];
            } else {
                check_rand_2 = rng.gen();
                if check_rand_2 < avg_prob {
                    edge_series[l as usize][t as usize] = 1;
                }
            }
        }
    }

    edge_series
}