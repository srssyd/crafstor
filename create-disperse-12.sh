gluster volume create disperse-12-4 disperse redundancy 4 transport rdma tfs00:/disk/disk{0..11}/disperse-12 force
gluster volume start disperse-12-4
gluster volume set disperse-12-4 disperse.coding-threads 8
gluster volume set disperse-12-4 performance.io-cache off
gluster volume set disperse-12-4 performance.write-behind off
