### mcs_data_rend, moon's data render

we need to produce new data from one data exist, according some application logic usually.

e.g. we queried some database data, store them in a hdf node name "datanode"
and we need to produce application hdf node called "outnode"
we need to change the datanode's key name, or, remove some key useless.

so, there is mcs_data_rend(), which can do this through config file,

you can read ldml_render() of mgate's pub/ directory for it's useage.


### function detail

* prototype

```c
NEOERR* mcs_data_rend(HDF *confignode, HDF *datanode, HDF *outnode);
```

* parameter confignode
    config node, usally exist in config file

* parameter datanode
    data node, usually queried from database

* parameter outnode
    out node, which you wanted

### directions in confignode

#### confignode's attribute
 * type
      data type(int, string, object, array, etc) of this outnode
      refer CnodeType of mcs.h for detail value

 * value(equal to confignode's value)
      outnode's value = hdf_get_value(datanode, value)
      value=__value__ for the whole datanode

 * require=true
      return error if hdf_get_value(datanode, value) == NULL

 * default
      default value if hdf_get_value(datanode, value) == NULL

 * valuenode=true
      outnode's value = hdf_obj_value(confignode)

 * childtype=__single__
      don't iteral datanode, appeared in array node only

#### confignode's name
 * __arraynode__
     array node's child name MUST be __arraynode__,
     and, __arrynode__ just can appeare as arraynode's child name

 * __datanode__
     outnode's name = hdf_obj_name(datanode)

#### confignode's value
 * .$.
     if ".$." exist in confignode's value, and this node type is array
     we will iteral two datanodes before and after ".$."

### example

* confignode

```
    boardid [type=102, require=true] = bid
    cardids [type=108, value=cids] {
        __arraynode__ [type=102] = __value__
    }
    eggids [type=108] = eids
    eggids {
        __arraynode__ [type=107, value=__value__] {
            eid [type=102, default=0] = id
        }
    }

    video [type=107, value=v] {
        restid [type=102]  = rid
        destids [type=108, value=dids] {
            __arraynode__ [type=102] = __value__
        }
    }

    p [type=107] = Query.p
    intime [type=106] = _NOW

    slotid [type=108, value=adgroups.$.spots] {
        __arraynode__ [type=102] = spot_id
    }
```

* datanode

```
    bid = 110
    cids {
        0 = 11
        1 = 12
    }
    eids {
        0 {
            id = 31
            name = one
        }
        1 {
            id = 32
            name = two
        }
    }
    v {
        rid = 2
        title = 闺密争抢土豪现场痛苦
        dids {
            0 = 21
            1 = 22
        }
    }

    adgroups {
      0 {
        attached_position [type="100"]  = 1
        spots [type="108"]  = foo
        spots {
          0 {
            delivery {
              delivery_id [type="102"]  = 143
              monitor_urls {
                show_monitor_urls [type="108"]  = foo
                show_monitor_urls {
                  0 {
                    url [type="100"]  = http://y.da.hunantv.com/json/cst/ip?s=$_sid$&d=663382085&v=1065006&cd=143&s=119&b=4580&ct=94
                    time [type="102"]  = 0
                  }
                }
                click_monitor_urls [type="108"]  = foo
                click_monitor_urls {
                  0 [type="100"]  = http://y.da.hunantv.com/json/cst/ck?s=$_sid$&d=663382085&v=1065006&cd=143&s=119&b=4580&ct=94
                }
              }
              creative {
                creative_format [type="100"]  = attached
                creative_id [type="102"]  = 94
                resource {
                  play_url [type="100"]  = http://test.ad.com/uploads/1419225729.mp4
                  height [type="100"]  = 288
                  width [type="100"]  = 512
                  redirect_url [type="100"]  = http://www.pingwest.com
                  play_time [type="100"]  = 15
                  type [type="100"]  = video/mp4
                }
                weight [type="102"]  = 0
              }
            }
            spot_id [type="100"]  = 119
          }

          1 {
            delivery {
              delivery_id [type="102"]  = 143
              monitor_urls {
                show_monitor_urls [type="108"]  = foo
                show_monitor_urls {
                  0 {
                    url [type="100"]  = http://y.da.hunantv.com/json/cst/ip?s=$_sid$&d=663382085&v=1065006&cd=143&s=119&b=4580&ct=94
                    time [type="102"]  = 0
                  }
                }
                click_monitor_urls [type="108"]  = foo
                click_monitor_urls {
                  0 [type="100"]  = http://y.da.hunantv.com/json/cst/ck?s=$_sid$&d=663382085&v=1065006&cd=143&s=119&b=4580&ct=94
                }
              }
              creative {
                creative_format [type="100"]  = attached
                creative_id [type="102"]  = 94
                resource {
                  play_url [type="100"]  = http://test.ad.com/uploads/1419225729.mp4
                  height [type="100"]  = 288
                  width [type="100"]  = 512
                  redirect_url [type="100"]  = http://www.pingwest.com
                  play_time [type="100"]  = 15
                  type [type="100"]  = video/mp4
                }
                weight [type="102"]  = 0
              }
            }
            spot_id [type="100"]  = 120
          }
        }
      }

      1 {
        attached_position [type="100"]  = 1
        spots [type="108"]  = foo
        spots {
          0 {
            delivery {
              delivery_id [type="102"]  = 143
              monitor_urls {
                show_monitor_urls [type="108"]  = foo
                show_monitor_urls {
                  0 {
                    url [type="100"]  = http://y.da.hunantv.com/json/cst/ip?s=$_sid$&d=663382085&v=1065006&cd=143&s=119&b=4580&ct=94
                    time [type="102"]  = 0
                  }
                }
                click_monitor_urls [type="108"]  = foo
                click_monitor_urls {
                  0 [type="100"]  = http://y.da.hunantv.com/json/cst/ck?s=$_sid$&d=663382085&v=1065006&cd=143&s=119&b=4580&ct=94
                }
              }
              creative {
                creative_format [type="100"]  = attached
                creative_id [type="102"]  = 94
                resource {
                  play_url [type="100"]  = http://test.ad.com/uploads/1419225729.mp4
                  height [type="100"]  = 288
                  width [type="100"]  = 512
                  redirect_url [type="100"]  = http://www.pingwest.com
                  play_time [type="100"]  = 15
                  type [type="100"]  = video/mp4
                }
                weight [type="102"]  = 0
              }
            }
            spot_id [type="100"]  = 121
          }
        }
      }
    }

    Query {
      p {
        m.p = 12
        u.id = 222
      }
    }
```


* outnode

```
    boardid [type="102"]  = 110
    cardids {
      0 [type="102"]  = 11
      1 [type="102"]  = 12
    }
    eggids {
      0 {
        eid [type="102"]  = 31
      }
      1 {
        eid [type="102"]  = 32
      }
    }
    video {
      restid [type="102"]  = 2
      destids {
        0 [type="102"]  = 21
        1 [type="102"]  = 22
      }
    }
    p {
      m {
        p = 12
      }
      u {
        id = 222
      }
    }
    intime [type="106"]  = 1421928793
    slotid {
      0 [type="102"]  = 119
      1 [type="102"]  = 120
      2 [type="102"]  = 121
    }
```

### demo in /doc/demo/hdfrend
