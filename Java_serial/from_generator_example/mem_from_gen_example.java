class mem_from_gen_example {

    public mem_from_gen_example() {

    }

    public static void main(String[] args) {

        int N = 20;
        int p = 4;
        double q = 0.9;
        double y = 0.1;
        int T = 100100;

        int max_order = 10;
        int min_order = 0;

        boolean[][] edge_series = DARNp_generator.generate_DARNp_net(N, p, q, y, T);
        int order_est = memory_estimators_with_co_order_coll.get_network_effective_memory(edge_series, max_order, min_order);

        System.out.println(order_est);
    }

}