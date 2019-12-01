import java.util.*;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;

class temp_net_parser {

    public temp_net_parser() {

    }


    public static boolean[][] parse_edge_file(String file_path, String separator, boolean has_headline) {
        
        Integer source, target, time_val;
        
        int time_len = 0;
        int num_links = 0;
        ArrayList<Integer> dum_link = new ArrayList<Integer>(Collections.nCopies(2, 0));
        HashSet<ArrayList<Integer> > seen_links = new HashSet<ArrayList<Integer> >();

        //ArrayList<Integer> curr_edge = new ArrayList<Integer>(Collections.nCopies(3, 0));
        ArrayList<ArrayList<Integer> > edge_list = new ArrayList<ArrayList<Integer> >();

        BufferedReader reader = null;

        try {
            File file = new File(file_path);
            reader = new BufferedReader(new FileReader(file));

            //If there is a header line, then ignore it.
            if(has_headline) {
                reader.readLine();
            }

            String line;
            while ((line = reader.readLine()) != null) {
                String[] parts = line.split(separator);
                source = Integer.parseInt(parts[0]);
                target = Integer.parseInt(parts[1]);
                if(source < target) {
                    dum_link.set(0,source);
                    dum_link.set(1,target);
                }
                else {
                    dum_link.set(0,target);
                    dum_link.set(1,source);
                }
                //Keep track of how many links are seen.
                if(!seen_links.contains(dum_link)) {
                    seen_links.add(dum_link);
                    num_links += 1;
                }
                
                time_val = Integer.parseInt(parts[2]);
                if(time_len < time_val) {
                    time_len = time_val;
                }
                ArrayList<Integer> curr_edge = new ArrayList<Integer>(Collections.nCopies(3, 0));
                curr_edge.set(0,source);
                curr_edge.set(1,target);
                curr_edge.set(2,time_val);
                edge_list.add(curr_edge);
            }
        } 
        catch (IOException e) {
            e.printStackTrace();
        } 
        finally {
            try {
                reader.close();
            } 
            catch (IOException e) {
                e.printStackTrace();
            }
        }

        //Got the edge list, now we need to format it.
        boolean[][] edge_series = convert_edge_list_format(edge_list, num_links, time_len+1);

        return edge_series;
    }

    private static boolean[][] convert_edge_list_format(ArrayList<ArrayList<Integer> > edge_list, int num_links, int time_len) {

        boolean[][] edge_series = new boolean[num_links][time_len];

        HashMap<ArrayList<Integer>, Integer> link_numbering = new HashMap<ArrayList<Integer>, Integer>();
        ArrayList<Integer> cur_timed_link = new ArrayList<Integer>(Collections.nCopies(3, 0));
        ArrayList<Integer> dum_link = new ArrayList<Integer>(Collections.nCopies(2, 0));
        int link_count = 0;
        int time_val;

        for(int i=0 ; i<edge_list.size() ; i++) {
            cur_timed_link = edge_list.get(i);
            dum_link.set(0, cur_timed_link.get(0));
            dum_link.set(1, cur_timed_link.get(1));
            time_val = cur_timed_link.get(2);

            //System.out.println(cur_timed_link.get(0) + " " + cur_timed_link.get(1) + " " + cur_timed_link.get(2));

            if(link_numbering.containsKey(dum_link)) {
                edge_series[link_numbering.get(dum_link)][time_val] = true;
            }
            else {
                link_numbering.put(dum_link, link_count);
                edge_series[link_count][time_val] = true;
                link_count += 1;
            }
        }
        return edge_series;
    }

    public static void main(String[] args) {

    }

}