- Elements array takes up a huge amount... in the order of 500KB...
- get ride of element[nodeID - 1] and just not do that?
Allow creation of unlimited tags:
    - Single tags have even IDs
    - paired tags have uneven IDs
    - Error IDs can be the first x values?
        - Or add status to the result?

- Code generation of Status codes?
- In tag generation:
    - Instead of using offset as a number we can use it as a bitmask? Check godbolt if matters
