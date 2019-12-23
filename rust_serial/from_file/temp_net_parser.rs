use std::fs::File;
use std::io::{BufRead, BufReader};
use std::collections::HashMap;

pub fn parse_edge_file(file_path: String, seperator: char, has_header: bool) -> Vec<Vec<i8>> {

    let mut edge_list: Vec<Vec<i32>> =  Vec::new();
    let mut link_num = 0;
    let mut max_time = 0;

    let file = File::open(file_path).unwrap();
    let reader = BufReader::new(file);

    /*
    let mut head_line = String::new();
    if has_header {
       let _ = reader.read_line(&mut head_line);
    }
    */
    let mut seen_links = HashMap::<Vec<i32>,i32>::new();

    // Read the file line by line using the lines() iterator from std::io::BufRead.
    for (index,line) in reader.lines().enumerate() {

        if (index==0) && (has_header) {
            continue;
        }

        let line = line.unwrap(); // Ignore errors.
        // Show the line and its number.
        let dum_vec: Vec<i32> = line.to_string().split(seperator).map(|x| x.parse::<i32>().unwrap()).collect::<Vec<i32>>();
        let dum_link = dum_vec[0..2].to_vec();
        
        if dum_vec[2] > max_time {
            max_time = dum_vec[2];
        }

        if !seen_links.contains_key(&dum_link) {
            seen_links.insert(dum_link, link_num);
            link_num += 1;
        }           
        edge_list.push(dum_vec);
    }
    let mut edge_series: Vec<Vec<i8>> = vec![vec![0;(max_time+1) as usize]; link_num as usize]; 
    

    //TODO!!!
    //Need to have a map from links observed to some index, then can fill the series.
    let mut time_val:i32 ;
    let mut edge_val: i32;
    for edge in edge_list {
        time_val = edge[2];
        edge_val = seen_links[&edge[0..2]];
        edge_series[edge_val as usize][time_val as usize] = 1;
    }
    edge_series
}
