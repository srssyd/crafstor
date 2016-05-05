gluster volume create disperse-64-16 disperse redundancy 16 transport rdma tfs00:/disk/disk{0..10}/disperse-64 tfs01:/disk/disk{0..10}/disperse-64 tfs02:/disk/disk{0..10}/disperse-64 tfs03:/disk/disk{0..10}/disperse-64 tfs04:/disk/disk{0..9}/disperse-64 tfs05:/disk/disk{0..9}/disperse-64 force
gluster volume start disperse-64-16
gluster volume set disperse-64-16 disperse.coding-threads 8
gluster volume set disperse-64-16 performance.io-cache off
gluster volume set disperse-64-16 performance.write-behind off
