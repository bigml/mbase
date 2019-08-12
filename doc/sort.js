let nodeData = [
      {
        id: 1,
        name: '开始',
        next: '2,3,4'
      },
      {
        id: 2,
        name: '2号',
        next: ''
      },
      {
        id: 3,
        name: '3号',
        next: '5,8'
      },
      {
        id: 4,
        name: '4号',
        next: '6'
      },
      {
        id: 5,
        name: '5号',
        next: '7'
      },
      {
        id: 6,
        name: '6号',
        next: ''
      },
      {
        id: 7,
        name: '7号',
        next: '1,3,5'
      },
      {
        id: 8,
        name: '8号',
        next: ''
      }
    ];

let filled = [];

let findnode = function(nodes, id) {
    for (let i = 0; i < nodes.length; i++) {
        if (parseInt(nodes[i].id) === parseInt(id)) return nodes[i];
    }

    return false;
}

let fillnode = function(backnodes, nodes, next) {
    console.log(filled);
    if (next.length > 0) {
        let ids = next.split(',');
        for (let idx = 0; idx < ids.length; idx++) {
            let id = ids[idx];
            console.log('处理子节点 ' + id);
            if (filled.indexOf(id) != -1) {
                // 回指的情况
                console.log('回指')
                backnodes.push(id);
            } else {
                // 子节点的情况
                let oridata = findnode(nodeData, id);
                console.log(oridata);
                if (typeof(oridata) === 'object') {
                    let node = {id: oridata.id, name: oridata.name, backpoints: [], childs: []};
                    nodes.push(node);
                    filled.push(id);
                    fillnode(node.backpoints, node.childs, oridata.next);
                }
            }
        }
    }
}

let sdata = {id: nodeData[0].id, name: nodeData[0].name, backpoints: [], childs: []};
filled.push('1');
fillnode(sdata.backpoints, sdata.childs, nodeData[0].next);
