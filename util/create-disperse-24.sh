gluster peer probe tfs03
gluster peer probe tfs05
gluster volume create disperse-24-8 disperse redundancy 8 tfs03:/data/bricks{0..11}/data_remote tfs05:/data/brick{0..11}/data_remote force
gluster volume start disperse-24-8
