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

#### simpler

* confignode

```
    boardid [type=102, require=true] = bid
    p [type=107] = Query.p
    intime [type=106] = _NOW
```

* datanode

```
    bid = 110
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
    p {
      m {
        p = 12
      }
      u {
        id = 222
      }
    }
    intime [type="106"]  = 1421928793
```

#### complexer

* confignode

```
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

    slotid [type=108, value=adgroups.$.spots] {
        __arraynode__ [type=102] = spot_id
    }
```

* datanode

```
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
        spots {
          0 {
            height [type="100"]  = 288
            spot_id [type="100"]  = 119
          }

          1 {
            height [type="100"]  = 288
            spot_id [type="100"]  = 120
          }
        }
      }

      1 {
        spots {
          0 {
            height [type="100"]  = 288
            spot_id [type="100"]  = 121
          }
        }
      }
    }
```


* outnode

```
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
    slotid {
      0 [type="102"]  = 119
      1 [type="102"]  = 120
      2 [type="102"]  = 121
    }
```

### demo in /doc/demo/hdfrend