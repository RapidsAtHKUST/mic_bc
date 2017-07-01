#!/usr/bin/env bash

export KMP_AFFINITY=granularity=fine,compact
export MIC_ENV_PREFIX=PHI
export PHI_KMP_AFFINITY=granularity=fine,compact


base_dir="/d2/lwangay/workspace"
data_dir="${base_dir}/graphs"
bin_dir="${base_dir}/mic_bc/build"
bin_name="mic_bc"

flags="0x0044"
threshold="0.45"

normal_graphs=( "delaunay_n17.graph" "rgg_n_2_17_s0.graph" "luxembourg.osm.graph" "kron_g500-simple-logn16.graph " "generate/rmat_17.txt" "smallworld.graph" )
one_deg_graphs=("ca-HepTh.txt" "email-Enron.txt" "com-amazon.ungraph.txt" )

cd ${bin_dir}

for i in "${normal_graphs[@]}"
do
    echo ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "10%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "10%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "30%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "30%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "50%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "50%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "70%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "70%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "90%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "90%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

done

flags="0x0028"

for i in "${one_deg_graphs[@]}"
do
    echo ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "10%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "10%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "30%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "30%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "50%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "50%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "70%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "70%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "90%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "90%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

done
