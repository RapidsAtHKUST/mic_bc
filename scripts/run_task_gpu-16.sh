#!/usr/bin/env bash
#
#export KMP_AFFINITY=granularity=fine,compact
#export MIC_ENV_PREFIX=PHI
#export PHI_KMP_AFFINITY=granularity=fine,compact


base_dir="/d2/lwangay/workspace"
data_dir="${base_dir}/graphs"
bin_dir="${base_dir}/hybrid_BC"
bin_name="bc"

graphs=( "delaunay_n17.graph" "smallworld.graph" "email-Enron.txt" )

cd ${bin_dir}

for i in "${graphs[@]}"
do
    echo ${bin_dir}/${bin_name} -i ${data_dir}/$i -k "10%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    ${bin_dir}/${bin_name} -i ${data_dir}/$i -k "10%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo ${bin_dir}/${bin_name} -i ${data_dir}/$i -k "30%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    ${bin_dir}/${bin_name} -i ${data_dir}/$i -k "30%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo ${bin_dir}/${bin_name} -i ${data_dir}/$i -k "50%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    ${bin_dir}/${bin_name} -i ${data_dir}/$i -k "50%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo ${bin_dir}/${bin_name} -i ${data_dir}/$i -k "70%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    ${bin_dir}/${bin_name} -i ${data_dir}/$i -k "70%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo ${bin_dir}/${bin_name} -i ${data_dir}/$i -k "90%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    ${bin_dir}/${bin_name} -i ${data_dir}/$i -k "90%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
done