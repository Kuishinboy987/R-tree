from rtree_bindings import RTree, Rect

t = RTree()
t.insert(Rect(1, 1), 3.14, 100)       # payload=3.14, id=100
t.insert(Rect(2, 2), 6.28, 101)

res = t.search(Rect(0, 0, 5, 5))
for payload, id_ in res:
    print("payload:", payload, "id:", id_)

print('--------------')

t.delete(Rect(1, 1), 3.14, 100)
res = t.search(Rect(0, 0, 5, 5))
for payload, id_ in res:
    print("payload:", payload, "id:", id_)