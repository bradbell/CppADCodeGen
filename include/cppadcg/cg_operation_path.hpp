#ifndef CPPAD_CG_OPERATION_PATH_INCLUDED
#define CPPAD_CG_OPERATION_PATH_INCLUDED
/* --------------------------------------------------------------------------
 *  CppADCodeGen: C++ Algorithmic Differentiation with Source Code Generation:
 *    Copyright (C) 2012 Ciengis
 *
 *  CppADCodeGen is distributed under multiple licenses:
 *
 *   - Common Public License Version 1.0 (CPL1), and
 *   - GNU General Public License Version 2 (GPL2).
 *
 * CPL1 terms and conditions can be found in the file "epl-v10.txt", while
 * terms and conditions for the GPL2 can be found in the file "gpl2.txt".
 * ----------------------------------------------------------------------------
 * Author: Joao Leal
 */

namespace CppAD {

    template<class Base>
    struct OperationPathNode {
        size_t arg_index;
        OperationNode<Base>* node;

        inline OperationPathNode() :
            arg_index(0),
            node(NULL) {
        }

        inline OperationPathNode(OperationNode<Base>* node_, size_t arg_index_) :
            arg_index(arg_index_),
            node(node_) {
        }

    };

    template<class Base>
    inline std::vector<std::vector<OperationPathNode<Base> > > CodeHandler<Base>::findPaths(OperationNode<Base>& root,
                                                                                             OperationNode<Base>& code,
                                                                                             size_t max) {
        resetCounters();

        std::vector<std::vector<OperationPathNode<Base> > > found;

        if (max > 0) {
            std::vector<OperationPathNode<Base> > path2node;
            path2node.reserve(30);
            path2node.push_back(OperationPathNode<Base> (&root, 0));

            if (&root == &code) {
                found.push_back(path2node);
            } else {
                findPaths(path2node, code, found, max);
            }
        }

        return found;
    }

    template<class Base>
    inline void CodeHandler<Base>::findPaths(std::vector<OperationPathNode<Base> >& currPath,
                                             OperationNode<Base>& code,
                                             std::vector<std::vector<OperationPathNode<Base> > >& found,
                                             size_t max) {

        OperationNode<Base>* currNode = currPath.back().node;
        if (&code == currNode) {
            found.push_back(currPath);
            return;
        }

        const std::vector<Argument<Base> >& args = currNode->getArguments();
        if (args.empty())
            return; // nothing to look in

        if (currNode->usageCount() > 0) {
            // already searched inside this node
            // any match would have been saved in found
            std::vector<SourceCodePath> pathsFromNode = findPathsFromNode(found, *currNode);
            typename std::vector<std::vector<OperationPathNode<Base> > >::const_iterator it;
            for (it = pathsFromNode.begin(); it != pathsFromNode.end(); ++it) {
                const SourceCodePath& pathFromNode = *it;
                SourceCodePath newPath(currPath.size() + pathFromNode.size());
                std::copy(currPath.begin(), currPath.end(), newPath.begin());
                std::copy(pathFromNode.begin(), pathFromNode.end(), newPath.begin() + currPath.size());
                found.push_back(newPath);
            }

        } else {
            // not visited yet
            currNode->increaseUsageCount(); // mark node as visited

            size_t size = args.size();
            for (size_t i = 0; i < size; ++i) {
                OperationNode<Base>* a = args[i].getOperation();
                if (a != NULL) {
                    currPath.push_back(OperationPathNode<Base> (a, i));
                    findPaths(currPath, code, found, max);
                    currPath.pop_back();
                    if (found.size() == max) {
                        return;
                    }
                }
            }
        }
    }

    template<class Base>
    inline std::vector<std::vector<OperationPathNode<Base> > > CodeHandler<Base>::findPathsFromNode(const std::vector<std::vector<OperationPathNode<Base> > > nodePaths,
                                                                                                     OperationNode<Base>& node) {

        std::vector<SourceCodePath> foundPaths;
        std::set<size_t> argsFound;

        typename std::vector<SourceCodePath>::const_iterator it;
        for (it = nodePaths.begin(); it != nodePaths.end(); ++it) {
            const SourceCodePath& path = *it;
            size_t size = path.size();
            for (size_t i = 0; i < size - 1; i++) {
                const OperationPathNode<Base>& pnode = path[i];
                if (pnode.node == &node) {
                    if (argsFound.find(path[i + 1].arg_index) == argsFound.end()) {
                        foundPaths.push_back(SourceCodePath(path.begin() + i + 1, path.end()));
                        argsFound.insert(path[i + 1].arg_index);
                    }
                }
            }
        }

        return foundPaths;

    }
}

#endif