/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2020 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not visit scipopt.org.         */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   xternal.c
 * @brief  main document page
 * @author Tobias Achterberg
 * @author Timo Berthold
 * @author Tristan Gally
 * @author Gerald Gamrath
 * @author Stefan Heinz
 * @author Gregor Hendel
 * @author Mathias Kinder
 * @author Marc Pfetsch
 * @author Stefan Vigerske
 * @author Robert Waniek
 * @author Kati Wolter
 * @author Michael Winkler
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/** @mainpage Overview
 *
 * @section WHATISSCIP What is SCIP?
 *
 * \SCIP is a framework to solve constraint integer programs (CIPs) and mixed-integer nonlinear programs. In particular,
 *
 * - \SCIP incorporates a mixed-integer programming (MIP) solver as well as
 * - an LP based mixed-integer nonlinear programming (MINLP) solver, and
 * - is a framework for branch-and-cut-and-price.
 *
 * See the web site of <a href="http://scipopt.org">\SCIP</a> for more information about licensing and to download \SCIP.
 *
 *
 * @section TABLEOFCONTENTS Structure of this manual
 *
 * This manual gives an accessible introduction to the functionality of the SCIP code in the following chapters
 *
 *  - @subpage GETTINGSTARTED      Installation and license information and an interactive shell tutorial
 *  - @subpage EXAMPLES            Coding examples in C and C++ in the source code distribution
 *  - @subpage APPLICATIONS        Extensions of SCIP for specific applications
 *  - @subpage PARAMETERS          List of all SCIP parameters
 *  - @subpage PROGRAMMING         Important programming concepts for working with(in) SCIP.
 *  - @subpage HOWTOADD            Detailed guides for adding user plugins
 *  - @subpage HOWTOUSESECTION     Detailed guides for advanced SCIP topics
 *  - @subpage FAQ                 Frequently asked questions (FAQ)
 *  - @subpage CHG                 Release notes and changelog
 *  - @subpage AUTHORS             SCIP Authors
 *  - @subpage EXTERNALDOC         Links to external documentation
 *
 *
 * @section QUICKSTART Quickstart
 *
 *  Let's consider the following minimal example in LP format. A 4-variable problem with a single, general integer
 *  variable and three linear constraints
 *
 *  \verbinclude simple.lp
 *
 *  Saving this file as "simple.lp" allows to read it into SCIP and solve it.
 *
 * ```
 * scip -c "read simple.lp optimize quit"
 * ```
 * reads and optimizes this model in no time:
 *
 * \verbinclude output.log
 *
 * @version  7.0.1.3
 *
 * \image html scippy.png
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/** @page LPI Available implementations of the LP solver interface
 *
 * SCIP provides a range of different interfaces to LP solvers:
 *
 * LPI name | LP solver
 * ---------|----------
 * `spx`    | SoPlex
 * `cpx`    | IBM ILOG CPLEX
 * `xprs`   | FICO XPress
 * `grb`    | Gurobi (version at least 7.0.2 required)
 * `clp`    | CoinOR CLP (interface currently sometimes produces wrong results)
 * `glop`   | Google Glop (contained in OR-tools)
 * `msk`    | Mosek (version at least 7.0.0 required)
 * `qsopt`  | QSopt (experimental)
 * `none`   | disables LP solving entirely (not recommended; only for technical reasons)
 *
 * There are two different interfaces for SoPlex. The default one (`spx`) uses an updated interface that is provided
 * by SoPlex itself (since version 2.0), resulting in a slimmer LPI that is similiar to those for CPLEX or XPRESS.
 * The other one - `spx1` - is the older LPI that directly interfaces the internal simplex solver of SoPlex and
 * therefore needs to duplicate some features in the LPI that are already available in SoPlex itself. It lacks some
 * features like persistent scaling which are only available in the modern interface. Upcoming features may not be
 * supported. Old compilers might have difficulties with the new interface because some C++11 features are required
 * that may not be supported.
 *
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/** @page NLPISOLVERS Available implementations of the NLP solver interface
 *
 * SCIP implements the NLP solver interface for the solvers <a href="https://projects.coin-or.org/Ipopt">IPOPT</a>, <a
 * href="https://worhp.de/">WORHP</a>, and <a href=" http://www.mcs.anl.gov/~leyffer/solvers.html">FilterSQP</a>. In
 * contrast to the implementations of the LP solver interface, SCIP can be compiled with multiple NLP solvers and selects
 * the solver with the highest priority at the beginning of the solving process.
 * Currently, the priorities are, in descending order: Ipopt, WORHP/IP, FilterSQP, WORHP/SQP.
 *
 * If more than one solver is available, then it is possible to solve all NLPs during the solving process with all
 * available NLP solvers by setting the parameter `nlpi/all/priority` to the highest value.
 * In this case, SCIP uses the solution from a solver that provides the best objective value. Other possible use
 * cases for the availability of multiple solvers have not been implemented yet.
 *
 * In the @ref MAKE "GNU make" based build system, building the implementations of the interface for FilterSQP, IPOPT, and
 * WORHP can be enabled by specifying `FILTERSQP=true`, `IPOPT=true`, and `WORHP=true`, respectively, as argument to the
 * `make` call.
 * In the @ref CMAKE "CMAKE" based build system, building the implementation of the interface for IPOPT and WORHP can be
 * enabled by specifying `IPOPT=on` and `WORHP=on`, respectively, as argument to the `cmake` call.
 *
 * @section NLPISOLVERS_IPOPT IPOPT
 *
 * <b>IPOPT</b> implements a primal-dual interior point method and uses line searches based on filter methods. It has
 * been developed by Andreas W&auml;chter and Carl Laird and is available under the Eclipse Public License on <a
 * href="https://www.coin-or.org/">COIN-OR</a>.
 *
 * @section NLPISOLVERS_WORHP WORHP
 *
 * <b>WORHP</b> implements a sequential quadratic programming method and a penalty-interior point algorithm.  It is
 * developed at the <a href="http://www.uni-bremen.de/en.html">University of Bremen</a> and is free for academic
 * purposes.
 *
 * @section NLPISOLVERS_FILTERSQP FilterSQP
 *
 * <b>FilterSQP</b> implements a sequential quadratic programming method. It has been developed by Roger Fletcher
 * and Sven Leyffer. It is not publicly available, but may be obtained from Sven Leyffer on request.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page GETTINGSTARTED Getting started
 *
 * - @subpage WHATPROBLEMS "What types of optimization problems does SCIP solve?"
 *
 * - @subpage LICENSE "License"
 * - @subpage INSTALL "Installation"
 * - @subpage SHELL       "Tutorial: the interactive shell"
 * - @subpage FILEREADERS "Readable file formats"
 * - @subpage INTERFACES "Interfaces"
 * - @subpage START       "How to start a new project"
 * - @subpage DOC         "How to search the documentation for interface methods"
 */

/**@page INSTALL Installing SCIP
 *
 * This chapter is a detailed guide to the installation procedure of SCIP.
 *
 * SCIP lets you freely choose between its own, manually maintained Makefile system
 * or the CMake cross platform build system generator.
 *
 * <b>For new users and on for installation of the scipoptsuite on windows, we strongly recommend to use CMake, if available on their targeted platform.</b>
 *
 * Which one you choose depends on you use case and your level of expertise.
 * If you just want to use SCIP as a black box solver you should use an installer with a precompiled binary from the <a href="http://scipopt.org/#download">download section</a>.
 * <b>This is highly recommended for new users.</b>
 * If you are just curious about SCIP and want to try it out you can use the <a href="http://www.pokutta.com/blog/pages/scip/scip-teaching.html"> dockerized SCIP container</a>.
 *
 * However if you want to develop your own plugin for scip you have to compile the SCIPOptSuite from the source code, which is available as a tarball from the <a href="http://scipopt.org/#download">website</a>.
 * Note that you might need some level of experience to be able to do this, this is described in the following.
 *
 * Please note that there are differences between both systems, most notably, the generated
 * library libscip will not be compatible between the versions. For more information, we
 * refer to the INSTALL file of the SCIP source code distribution.
 *
 * - @subpage CMAKE   "Installation information using CMake (recommended for new users)"
 * - @subpage MAKE    "Installation information using Makefiles"
 * - @subpage LPI         "Available implementations of the LP solver interface"
 * - @subpage NLPISOLVERS "Available implementations of the NLP solver interface"
 * - @subpage INSTALL_APPLICATIONS_EXAMPLES "Installation of applications and examples"
 */

/**@page PROGRAMMING Programming with SCIP
 *
 * - @subpage CODE    "Coding style guidelines"
 * - @subpage OBJ     "Creating, capturing, releasing, and adding data objects"
 * - @subpage MEMORY  "Using the memory functions of SCIP"
 * - @subpage DEBUG   "Debugging"
 */
/**@page HOWTOADD How to add ...
 *
 * Below you find for most plugin types a detailed description of how to implement and add them to \SCIP.
 *
 * - @subpage CONS    "Constraint handlers"
 * - @subpage PRICER  "Variable pricers"
 * - @subpage PRESOL  "Presolvers"
 * - @subpage SEPA    "Separators"
 * - @subpage PROP    "Propagators"
 * - @subpage BRANCH  "Branching rules"
 * - @subpage CUTSEL  "Cut selectors"
 * - @subpage NODESEL "Node selectors"
 * - @subpage HEUR    "Primal heuristics"
 * - @subpage DIVINGHEUR "Diving heuristics"
 * - @subpage RELAX   "Relaxation handlers"
 * - @subpage READER  "File readers"
 * - @subpage DIALOG  "Dialogs"
 * - @subpage DISP    "Display columns"
 * - @subpage EVENT   "Event handler"
 * - @subpage NLPI    "Interface to NLP solvers"
 * - @subpage EXPRINT "Interfaces to expression interpreters"
 * - @subpage PARAM   "additional user parameters"
 * - @subpage TABLE   "Statistics tables"
 * - @subpage BENDER "Benders' decomposition"
 *   + @subpage BENDERSCUT "Benders' decomposition cuts"
 */
/**@page HOWTOUSESECTION How to use ...
 *
 * - @subpage CONF    "Conflict analysis"
 * - @subpage TEST    "How to run automated tests with SCIP"
 * - @subpage COUNTER "How to use SCIP to count feasible solutions"
 * - @subpage REOPT   "How to use reoptimization in SCIP"
 * - @subpage CONCSCIP "How to use the concurrent solving mode in SCIP"
 * - @subpage DECOMP "How to provide a problem decomposition"
 * - @subpage BENDDECF "How to use the Benders' decomposition framework"
 * - @subpage TRAINESTIMATION "How to train custom tree size estimation for SCIP"
 */

/**@page AUTHORS SCIP Authors
 *
 * A list of all current and former developers as well as contributors can
 * be found on the
 * <a class="el" href="http://scipopt.org/#developers">Main Web Page</a>.
 *
 */

/**@page EXTERNALDOC  Links to external documentation
 *
 * <a class="el" href="https://www.cgudapati.com/integer-programming/2019/12/15/Getting-Started-With-SCIP-Optimization-Suite.html">Getting Started with SCIP optimization in C++: A toy example</a> by Chaitanya Gudapati.
 *
 */

/**@page CHG Release notes and changes between different versions of SCIP
 *
 * New features, peformance improvements, and interface changes between different versions of SCIP are documented in the
 * release notes:
 *
 * - \subpage RN70         "SCIP 7.0"
 * - \subpage RN60         "SCIP 6.0"
 * - \subpage RN50         "SCIP 5.0"
 * - \subpage RN40         "SCIP 4.0"
 * - \subpage RN32         "SCIP 3.2"
 * - \subpage RN31         "SCIP 3.1"
 * - \subpage RN30         "SCIP 3.0"
 * - \subpage RN21         "SCIP 2.1"
 * - \subpage RN20         "SCIP 2.0"
 * - \subpage RN12         "SCIP 1.2"
 * - \subpage RN11         "SCIP 1.1"
 * - \subpage RN10         "SCIP 1.0"
 * - \subpage RN09         "SCIP 0.9"
 * - \subpage RN08         "SCIP 0.8"
 * - \subpage RN07         "SCIP 0.7"
 *
 */

/**@page WHATPROBLEMS What types of optimization problems does SCIP solve?
 *
 * As a stand-alone solver, \SCIP can solve mixed-integer nonlinear programs \b (MINLPs), to which it applies
 * an LP based spatial branch-and-cut algorithm. This method is guaranteed to solve bounded MINLPs
 * within a given numerical tolerance in a finite amount of time. In particular, \SCIP is a stand-alone
 * solver for mixed-integer linear programs \b (MIPs).
 *
 * As a framework, \SCIP also provides the tools to solve constraint optimization problems defined over
 * integer and continuous variables. Therefore, the design of \SCIP
 * supports the easy integration of constraints of arbitrary type into the solver.
 * More precisely, \SCIP can handle the class of constraint integer programs \b (CIPs), which are constraint optimization problems
 * that become linear programs (LPs) after the integer variables are fixed.
 *
 * @section PROBLEMCLASSES Some important subclasses of CIP and MINLP
 *
 * The following table gives a non-exhaustive list of common types of mathematical optimization problems that can be solved
 * through \SCIP itself or one of its extensions. Some recommendations are given on how to compile \SCIP for a
 * certain problem class and how make best use of \SCIP. The file format column gives some common file
 * formats for every class. Note that since some of the mentioned problem classes are more general than others (like
 * every LP is a MIP is an MINLP), the formats for the superclass should always work just as fine, although they
 * may be less common for the class at hand.
 *
 * Please see also the pages on \ref EXAMPLES "SCIP Examples" and \ref APPLICATIONS "SCIP Applications" to learn more on how
 * to extend \SCIP for a particular MIP, MINLP, or CIP application.
 * All examples and applications use the C or C++ APIs of \SCIP. Please have a look at \ref INTERFACES "SCIP interfaces"
 * to see how to use \SCIP from within other programming languages.
 *
 * <table class="types">
 * <caption align="top">Some problem classes that can be solved by \SCIP</caption>
 *    <tr style="border-bottom: medium solid black;">
 *       <th>Problem class</th>
 *       <th>Mathematical problem description</th>
 *       <th>Supported file formats</th>
 *       <th>Recommendations</th>
 *    </tr>
 * <tr>
 *    <td>Mixed-integer linear program (MIP)</td>
 *    <td>\f{align*}{
 *            \text{min} \quad& c^T x \\
 *            \text{s.t.} \quad& Ax \geq b \\
 *            &l_{j} \leq x_{j} \leq u_{j} && \forall j \in \mathcal{N} \\
 *            &x_{j} \in \mathbb{Z} && \forall j \in \mathcal{I}
 *        \f}
 *    </td>
 *    <td>
 *       <ul>
 *          <li>\ref reader_cip.h "CIP"</li>
 *          <li>\ref reader_mps.h "MPS"</li>
 *          <li>\ref reader_lp.h "LP"</li>
 *          <li>\ref reader_zpl.h "ZPL"</li>
 *       </ul>
 *    </td>
 *    <td>
 *       <ul>
 *          <li>\SCIP requires an external LP solver to solve LP relaxations, which needs to be specified
 *          at compilation time. By default, it uses SoPlex (<code>LPS=spx</code>). See \ref MAKE for a
 *          list of available LP solver interfaces and how to use them inside \SCIP.</li>
 *          <li>Compile with Zimpl support (<code>ZIMPL=true</code>) to read in Zimpl models directly.</li>
 *          <li>\SCIP comes with many different parameters. Use the provided emphasis settings (see \ref SHELL "this tutorial")
 *          to change many parameters at once and boost the performance.</li>
 *          <li>Test instances are available at <code>check/instances/MIP/</code>.</li>
 *       </ul>
 *    </td>
 * </tr>
 * <tr>
 *    <td>Mixed-integer nonlinear program (MINLP)</td>
 *    <td>\f{align*}{
 *            \text{min} \quad& f(x) \\
 *            \text{s.t.} \quad& g_{i}(x) \leq 0 && \forall i \in \mathcal{M} \\
 *            &l_{j} \leq x_{j} \leq u_{j} && \forall j \in \mathcal{N} \\
 *            &x_{j} \in \mathbb{Z} && \forall j \in \mathcal{I}
 *        \f}
 *    </td>
 *    <td>
 *        <ul>
 *          <li>\ref reader_cip.h "CIP"</li>
 *          <li>\ref reader_gms.h "GMS"</li>
 *          <li>\ref reader_osil.h "OSiL"</li>
 *          <li>\ref reader_pip.h "PIP"</li>
 *          <li>\ref reader_zpl.h "ZPL"</li>
 *          <li>For MIQCPS:
 *             <ul>
 *                <li>\ref reader_lp.h "LP"</li>
 *                <li>\ref reader_mps.h "MPS"</li>
 *             </ul>
 *          </li>
 *        </ul>
 *    </td>
 *    <td>
 *       <ul>
 *          <li>Compile with <code>IPOPT=true</code> for better performance.</li>
 *          <li>Compile with <code>WORHP=true</code> for better performance.</li>
 *          <li>Compile with <code>FILTERSQP=true</code> for better performance.</li>
 *          <li>See <a href="FAQ\FILEEXT#minlptypes"> Which kind of MINLPs are supported by \SCIP? </a> in the FAQ.</li>
 *          <li>There is an interface for the modelling language AMPL, see \ref INTERFACES.</li>
 *          <li>Mixed-integer quadratically constrained programs (MIQCP) can also be formulated in the file formats
 *             <ul>
 *                <li>\ref reader_lp.h "LP", and</li>
 *                <li>\ref reader_mps.h "MPS".</li>
 *             </ul>
 *          </li>
 *          <li>Test instances are available at <code>check/instances/MINLP/</code>.</li>
 *       </ul>
 *    </td>
 * </td>
 * <tr>
 *    <td>Constraint Integer Program (CIP)</td>
 *    <td>\f{align*}{
 *            \text{min} \quad& c^T x + d^T y \\
 *            \text{s.t.} \quad& C_i(x,y) = \text{true} && \forall i \in \mathcal{M} \\
 *            & x \in \mathbb{Z}^{p}, y  \in \mathbb{R}^{n - p}
 *        \f}
 *        where \f$\forall i \in\mathcal{M}, \forall x^* \in \mathbb{Z}^{p},\f$ \f$ \{ y : C_i(x^*, y) = \text{true} \} \f$ is a polyhedron.
 *    </td>
 *    <td>
 *       <ul>
 *          <li>\ref reader_cip.h "CIP"</li>
 *          <li>\ref reader_fzn.h "FlatZinc"</li>
 *       </ul>
 *    </td>
 *    <td>
 *       <ul>
 *          <li>\SCIP supports a limited number of general constraints; see \ref CONS "How to add constraint handlers"
 *          to learn how to extend the \SCIP framework to a given CIP.</li>
 *          <li>Use the emphasis setting <code>set emphasis cpsolver</code> to completely disable LP solves and
 *          use depth-first search with periodic restarts, see also
 *          <a href="FAQ\FILEEXT#scipascpsolver"> Can I use \SCIP as a pure CP solver? </a> in the FAQ.</li>
 *          <li>Test instances are available at <code>check/instances/CP</code>.</li>
 *       </ul>
 *    </td>
 * <tr>
 *    <td>Convex MINLP</td>
 *    <td>Like MINLP, \f$f\f$ and all \f$g_i\f$ are \b convex.</td>
 *    <td>see MINLP formats</td>
 *    <td>
 *       <ul>
 *          <li>See the comments for MINLP.</li>
 *          <li>In addition, use <code>constraints/nonlinear/assumeconvex = TRUE</code> to inform \SCIP about a convex
 *          problem in cases where the automated detection is not strong enough.</li>
 *          <li>Test instances are available at <code>check/instances/MINLP/circle.cip</code>.</li>
 *       </ul>
 *    </td>
 * </td>
 * <tr>
 *    <td>Linear program (LP)</td>
 *    <td>\f{align*}{
 *            \text{min} \quad& c^T x \\
 *            \text{s.t.} \quad& Ax \geq b \\
 *            & x_{j} \geq 0 && \forall j \in \mathcal{N}
 *        \f}
 *    </td>
 *    <td>see MIP formats</td>
 *    <td>See <a href="FAQ\FILEEXT#scipaslpsolver">Can I use \SCIP as a pure LP solver</a> in the FAQ.</td>
 * </td>
 * <tr>
 *    <td>Pseudoboolean optimization</td>
 *    <td>\f{align*}{
 *            \text{min} \quad& c^T x \\
 *            \text{s.t.} \quad& \sum_{k=0}^p a_{ik} \cdot \prod_{j \in \mathcal{N}_{ik}} x_j \leq b_i && \forall i \in \mathcal{M} \\
 *            &x_{j} \in \{0,1\} && \forall j \in \mathcal{N}
 *        \f}
 *    </td>
 *    <td>
 *       <ul>
 *          <li>\ref reader_wbo.h "WBO"</li>
 *          <li>\ref reader_opb.h "OPB"</li>
 *       </ul>
 *    </td>
 *    <td>
 *       <ul>
 *          <li>Test instances are available at <code>check/instances/PseudoBoolean/</code>.</li>
 *       </ul>
 *    </td>
 * </tr>
 * <tr>
 *    <td>Satisfiability (SAT) and variants</td>
 *    <td>\f{align*}{
 *            \text{min} \quad& 0 \\
 *            \text{s.t.} \quad&\bigvee\limits_{j \in B_i} x_j \vee \bigvee\limits_{j \in \bar{B}_i} \neg x_j = \text{true} && \forall i \in \mathcal{M}\\
 *            &x_{j} \in \{\text{false},\text{true}\} && \forall j \in \mathcal{N}
 *        \f}
 *    </td>
 *    <td>
 *        <ul>
 *          <li>\ref reader_cnf.h "CNF"</li>
 *       </ul>
 *    </td>
 *    <td>
 *       <ul>
 *         <li>Use the emphasis setting <code>set emphasis cpsolver</code> to completely disable LP solves and
 *          use depth-first search with periodic restarts, see also
 *          <a href="FAQ\FILEEXT#scipascpsolver"> Can I use \SCIP as a pure CP/SAT solver? </a> in the FAQ.</li>
 *         <li>Test instances are available at <code>check/instances/SAT/</code>.</li>
 *       </ul>
 *    </td>
 * </tr>
 * <tr>
 *    <td>Multicriteria optimization</td>
 *    <td>\f{align*}{
 *         \text{min} \quad &(c_1^T x,\ldots,c_k^T x) \\
 *         \text{s.t. } \quad& Ax \geq b \\
 *         &x \in \mathbb{K}^n
 *          \f}
 *          where \f$\mathbb{K}\f$ is either \f$\mathbb{Z}\f$ or \f$\mathbb{R}\f$.
 *    </td>
 *    <td colspan="3"> see the <a href="http://polyscipopt.org/">PolySCIP web page</a></td>
 * </tr>
 * <tr>
 *    <td>Mixed-integer semidefinite program (MISDP)</td>
 *    <td>\f{align*}{
 *         \text{inf} \quad \thinspace & b^T y \\
 *         \text{s.t.} \quad & \sum_{j=1}^m A_j\, y_j - A_0 \succeq 0 \\
 *         & y_j \in \mathbb{Z} && \forall\, j \in \mathcal{I}
 *        \f}
 *    </td>
 *    <td colspan="3"> see the <a href="http://www.opt.tu-darmstadt.de/scipsdp/">SCIP-SDP web page</a></td>
 * </tr>
 * </table>
 *
 *
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page CODE Coding style guidelines
 *
 * We follow the following coding style guidelines and recommend them for all developers.
 *
 * @section CODESPACING Spacing:
 *
 * - Indentation is 3 spaces. No tabs anywhere in the code.
 * - Every opening parenthesis requires an additional indentation of 3 spaces.
 *
 *   @refsnippet{src/scip/branch_relpscost.c,SnippetCodeStyleParenIndent}
 *
 * - Spaces around all operators.
 * - Spaces around the arguments inside an if/for/while-statement, as well as inside macros (e.g., SCIP_CALL).
 * - No spaces between control structure keywords like "if", "for", "while", "switch" and the corresponding brackets.
 * - No spaces between a function name and the parenthesis in both the definition and function calls.
 * - Braces are on a new line and not indented.
 * - Braces in if/for-statements should only be omitted if they enclose a single line.
 *
 *   @refsnippet{src/scip/branch_relpscost.c,SnippetCodeStyleIfFor}
 *
 * - In function declarations, every parameter is on a new line. The name of the parameter starts at column 26,
 *   the comment starts at column 46 (if column-count starts with 1).
 * - Maximal line length is 120 characters.
 * - Always only one declaration in a line.
 * - Variable names should be all lower case.
 *
 *   @refsnippet{src/scip/branch_relpscost.c,SnippetCodeStyleDeclaration}
 *
 * - Blank lines are inserted where it improves readability.
 * - Multiple blank lines are used to structure the code where single blank lines are insufficient,
 *   e.g., between differrent sections of the code.
 *
 *   @refsnippet{src/scip/heur_xyz.c,SnippetCodeStyleBlanks}
 *
 * @section CODENAMING  Naming:
 *
 * - Use assert() to show preconditions for the parameters, invariants, and postconditions.
 * - Make all functions that are not used outside the module 'static'.
 * - Naming should start with a lower case letter.
 *
 *   @refsnippet{src/scip/branch_relpscost.c,SnippetCodeStyleStaticAsserts}
 *
 * - All global functions start with "SCIP". In the usual naming scheme this is followed by the object and a method name
 *   like in SCIPlpAddRow(). Functions return TRUE or FALSE should be named like SCIPisFeasEQ().
 *
 *   @refsnippet{src/scip/scip_numerics.h,SnippetCodeStyleNaming}
 *
 * - For each structure there is a typedef with the name in all upper case.
 * - Defines should be named all upper case.
 *
 *   @refsnippet{src/scip/type_set.h,SnippetCodeStyleExample}
 *
 * @section CODEDOC Documentation:
 *
 * - Document functions, parameters, and variables in a doxygen conformed way.
 * - Please do not leave code in comments that has been commented out, don't use `#if
 *   0`. Instead put the code within defines `#ifdef SCIP_DISABLED_CODE` and add an explanation.
 * - Todos need double stars to be registered by doxygen.
 * - When documenting methods, the first brief description starts with lower case and is separated by semi-colons, if necessary
 *   The longer description starts capitalized and consists of complete sentences.
 *   If the documentation consists of multiple lines, the comment end must be on a new line.
 *
 *   @refsnippet{src/scip/scip_datastructures.h,SnippetCodeStyleComment}
 *
 *
 * @section XEMACS Customize (x)emacs
 *
 * If you are using (x)emacs, you can use the following customization for the c++-mode. These settings satisfy the
 * coding guidelines of \SCIP.
 *
 * \include codestyle/emacs_scip_codestyle.el
 *
 *
 * @section ECLIPSE Customize eclipse
 *
 * Eclipse user can use the profile below. This profile does not match the \SCIP coding guideline completely.
 *
 * \include codestyle/eclipse_scip_codestyle.xml
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page CMAKE Building SCIP with CMake
 *
 * <a href=https://cmake.org/>CMake</a> is a build system generator that can create, e.g., Makefiles for UNIX and Mac
 * or Visual Studio project files for Windows.
 *
 * CMake provides an <a href="https://cmake.org/cmake/help/latest/manual/cmake.1.html">extensive documentation</a>
 * explaining available features and use cases as well as an <a href="https://cmake.org/Wiki/CMake_FAQ">FAQ section</a>.
 * It's recommended to use the latest stable CMake version available. `cmake --help` is also a good first step to see
 * available options and usage information.
 *
 * Platform independent build instructions:
 *
 * ```
 * cmake -Bbuild -H. [-DSOPLEX_DIR=/path/to/soplex]
 * cmake --build build --config Release
 * ```
 *
 * Linux/macOS Makefile-based build instructions:
 *
 * ```
 * mkdir build
 * cd build
 * cmake .. [-DSOPLEX_DIR=/path/to/soplex]
 * make
 * # optional: run a quick check on some instances
 * make check
 * # optional: install scip executable, library, and headers
 * make install
 * ```
 *
 * CMake uses an out-of-source build, i.e., compiled binaries and object files are separated from the source tree and
 * located in another directory. Usually this directory is called `build` or `debug` or whatever you prefer. From within
 * this directory, run `cmake <path/to/SCIP>` to configure your build, followed by `make` to compile the code according
 * to the current configuration (this assumes that you chose Linux Makefiles as CMake Generator). By default, SCIP
 * searches for Soplex as LP solver. If SoPlex is not installed systemwide, the path to a CMake build directory
 * of SoPlex must be specified (ie one that contains "soplex-config.cmake"). Alternatively, a different LP solver
 * can be specified with the `LPS` variable, see \ref CMAKE_CONFIG and \ref LPI.
 *
 * Afterwards,
 * successive calls to `make` are going to recompile modified source code,
 * without requiring another call to `cmake`. The initial configuration step checks your environment for available
 * third-party libraries and packages and sets up the configuration accordingly, e.g., disabling support for GMP if not
 * installed.
 *
 * The generated executable and libraries are put in directories `bin` and `lib` respectively and will simply be named
 * `scip` or `libscip.so`. This is different from the naming convention of the previous Makefile setup that
 * appended the configuration details like OS and third party dependencies directly to the name of the binary or library.
 * The CMake setup tries to follow the established Linux/UNIX compilation conventions to facilitate the use of the
 * libraries in other applications. The previously generated sub-libraries like `liblpi.so` or `libobjscip.so` are not
 * created by default anymore. They can be built using the respective targets `liblpi`, `libobjscip`, etc. The main
 * library `libscip.so` will contain all SCIP sources and won't have dependencies to the other sub-libs.
 *
 * @section CMAKE_CONFIG Modifying a CMake configuration
 *
 * There are several options that can be passed to the `cmake <path/to/SCIP>` call to modify how the code is built.
 * For all of these options and parameters you have to use `-D<Parameter_name>=<value>`. Following a list of available
 * options, for the full list run
 *
 * ```
 * cmake <path/to/SCIP> -LH
 * ```
 *
 * CMake option         | Available values               | Makefile equivalent    | Remarks                                    |
 * ---------------------|--------------------------------|------------------------|--------------------------------------------|
 * CMAKE_BUILD_TYPE     | Release, Debug, ...            | OPT=[opt, dbg]         |                                            |
 * GMP                  | on, off                        | GMP=[true, false]      | specify GMP_DIR if not found automatically |
 * IPOPT                | on, off                        | IPOPT=[true,false]     | requires IPOPT version >= 3.12.0; specify IPOPT_DIR if not found automatically |
 * LPS                  | spx, cpx, grb, xprs, ...       | LPS=...                | See \ref LPI for a complete list; specify SOPLEX_DIR, CPLEX_DIR, MOSEK_DIR, ... if LP solver is not found automatically |
 * SYM                  | bliss, none                    | --                     | for bliss, specify BLISS_DIR |
 * WORHP                | on, off                        | WORHP=[true,false]     | should worhp be linked; specify WORHP_DIR if not found automatically |
 * ZIMPL                | on, off                        | ZIMPL=[true, false]    | specify ZIMPL_DIR if not found automatically |
 * READLINE             | on, off                        | READLINE=[true, false] |                                            |
 * ..._DIR              | <custom/path/to/.../package>   | --                     | e.g. IPOPT_DIR, CPLEX_DIR, WORHP_DIR, Readline_DIR ...  |
 * CMAKE_INSTALL_PREFIX | \<path\>                       | INSTALLDIR=\<path\>    |                                            |
 * SHARED               | on, off                        | SHARED=[true, false]   |                                            |
 * CXXONLY              | on, off                        | --                     | use a C++ compiler for all source files    |
 * COVERAGE             | on, off                        | --                     | use with gcc, lcov, gcov in **debug** mode |
 * COVERAGE_CTEST_ARGS  | ctest argument string          | --                     | see `ctest --help` for arguments           |
 * DEBUGSOL             | on, off                        | DEBUGSOL=[true,false]  | specify a debugging solution by setting the "misc/debugsol" parameter of SCIP |
 * LPSCHECK             | on, off                        | LPSCHECK=[true,false]  | double check SoPlex results with CPLEX     |
 * NOBLKMEM             | on, off                        | NOBLKMEM=[true,false]  |                                            |
 * NOBUFMEM             | on, off                        | NOBUFMEM=[true,false]  |                                            |
 * NOBLKBUFMEM          | on, off                        | NOBLKBUFMEM=[true,false] |                                          |
 * MT                   | on, off                        |                        | use static runtime libraries for Visual Studio compiler on Windows |
 * THREADSAFE           | on, off                        | THREADSAFE=[true,false] | thread safe compilation                   |
 * SANITIZE_...         | on, off                        | --                     | enable sanitizer in debug mode if available |
 * TPI                  | tny, omp, none                 | TPI=[tny,omp,none]     | enable task processing interface required for concurrent solver |
 *
 * Parameters can be set all at once or in subsequent calls to `cmake` - extending or modifying the existing
 * configuration.
 *
 * @section CTEST Testing with CTest
 *
 * There is an extensive test suite written for <a href="https://cmake.org/cmake/help/latest/manual/ctest.1.html">CTest</a>,
 * that may take a while to complete. To perform a quick test to see whether the compilation was really successful you may
 * run `make check`. To see all available tests, run
 *
 * ```
 * ctest -N
 * ```
 *
 * and to perform a memory check, run
 *
 * ```
 * ctest -T MemCheck
 * ```
 *
 * If <a href="https://criterion.readthedocs.io/en/master/">Criterion</a> is installed (set
 * custom path with `-DCRITERION_DIR=<path>`) the target `unittests` can be used to compile and run the available unit tests.
 *
 * A coverage report for the entire test suite can be generated. This requires a modification of the
 * compilation process. Two variables govern the report generation, `COVERAGE` and `COVERAGE_CTEST_ARGS`.
 * It is recommended to use the Debug build type.
 *
 * ```
 * cmake .. -DCOVERAGE=on -DCOVERAGE_CTEST_ARGS="-R MIP -E stein -j4" -DCMAKE_BUILD_TYPE=Debug
 * ```
 *
 * In this example, coverage is enabled in combination with the build type Debug. In addition, only the coverage
 * for tests with "MIP" in the name are run, excluding those that have "stein" in the name.
 * The tests are performed in parallel using 4 cores.
 *
 * Use the `coverage` target, e.g., `make coverage`, to build the coverage report. The generated report can be found
 * under "coverage/index.html".
 *
 * @section CMAKE_INSTALL Installation
 *
 * CMake uses a default directory for installation, e.g., /usr/local on Linux. This can be modified by either changing
 * the configuration using `-DCMAKE_INSTALL_PREFIX` as explained in \ref CMAKE_CONFIG or by setting the environment
 * variable `DESTDIR` during or before the install command, e.g., `DESTDIR=<custom/install/dir> make install`.
 *
 * @section CMAKE_TARGETS Additional targets
 *
 * There are several further targets available, which can be listed using `make help`. For instance, there are some
 * examples that can be built with `make examples` or by specifying a certain one: `make <example-name>`.
 *
 * | CMake target    | Description                                           | Requirements                          |
 * |-----------------|-------------------------------------------------------|---------------------------------------|
 * | scip            | build SCIP executable                                 |                                       |
 * | applications    | build executables for all applications                |                                       |
 * | examples        | build executables for all examples                    |                                       |
 * | unittests       | build unit tests                                      | the Criterion package, see \ref CTEST |
 * | all_executables | build all of the above                                |                                       |
 * | libscip         | build the SCIP library                                |                                       |
 * | install         | install SCIP, see \ref CMAKE_INSTALL                  |                                       |
 * | coverage        | run the test suite and create a coverage report       | build flag `-DCOVERAGE=on`            |
 * | liblpi          | build the LPI library                                 |                                       |
 * | libnlpi         | build the NLPI library                                |                                       |
 * | libobjscip      | build the ObjSCIP library for the C++ wrapper classes |                                       |
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page MAKE Makefiles / Installation information
 *
 * <b>Please note, that the Makefile system is not actively maintained anymore.
 * If possible, please use \ref CMAKE "the cmake system".</b>
 *
 * In most cases (LINUX and MAC) it is quite easy to compile and install \SCIP. Therefore, reading the section
 * \ref BRIEFINSTALL "Brief installation description" should usually be enough. If this is not the case you find a
 * \ref DETAILEDINSTALL "Detailed installation description" below as well as \ref EXAMPLE1 "Examples".
 * We recommend using GCC version 4.8 or later.
 *
 * @section BRIEFINSTALL Brief installation description
 *
 * The easiest way to install \SCIP is to use the \SCIP Optimization Suite which contains \SCIP, SoPlex, and ZIMPL.
 * For that we refer to the INSTALL file of the \SCIP Optimization Suite (main advantage: there is no need
 * to specify any directories, the compiling process is fully automated).
 *
 * Compiling \SCIP directly can be done as follows:
 *
 * -# unpack the tarball <code>tar xvf scip-x.y.z.tgz</code>
 * -# change to the directory <code>cd scip-x.y.z</code>
 * -# start compiling \SCIP by typing <code>make</code>
 * -# (optional) install the header, libraries, and binary <code>make install INSTALLDIR="/usr/local/</code>
 *
 * During your first compilation you will be asked for some soft-link targets,
 * depending on the LP solver you want to use. Usually, \SCIP needs the
 * following information
 * -# the directory where the include files of the LP solver lie
 * -# the library file(s) "lib*.a" or/and "lib*.so"
 *
 * Besides that, \SCIP needs some soft-link targets, for ZIMPL
 * -# the directory where the include files of ZIMPL lie
 * -# the library file(s) "lib*.a" or/and "lib*.so"
 *
 * You will need either the .a or the .so files and can skip the others by
 * just pressing return.
 *
 * The most common compiling issue is that some libraries are missing
 * on your system or that they are outdated. \SCIP per default requires
 * zlib, gmp and readline.  Try compiling with: <code> make ZLIB=false
 * READLINE=false ZIMPL=false</code> or, better, install them. Note
 * that under Linux-based systems, you need to install the
 * developer-versions of gmp/zlib/readline, in order to also have the
 * header-files available.
 *
 @section DETAILEDINSTALL Detailed installation description
 *
 * In this section we describe the use, and a few features, of the \SCIP Makefile. We also give two examples for how to install
 * \SCIP. The \ref EXAMPLE1 "first example" illustrates the default installation. This means, with SoPleX and ZIMPL. The
 * \ref EXAMPLE2 "second example" shows how to get CPLEX linked to \SCIP without ZIMPL. This is followed by a section which
 * gives some hints on what to do if the \ref COMPILERPROBLEMS "compilation throws an error". We give some comments on
 * how to install \SCIP under \ref WINDOWS "WINDOWS" and show \ref RUN "how to start \SCIP".
 *
 * If you experience any problems during the installation, you will find help in the INSTALL file.
 *
 * \SCIP contains a makefile system, which allows the individual setting of several parameters. A detailed list of parameter settings
 * obtained by <code>make help</code>. For instance, the following settings are supported:
 *
 * - <code>OPT=\<dbg|opt\></code> Here <code>dbg</code> turns on the debug mode of \SCIP. This enables asserts
 *   and avoids macros for several function in order to ease debugging.
 *
 * - <code>LPS=\<clp|cpx|grb|msk|qso|spx|xprs|none\></code> This determines the LP-solver, which should be
 *   installed separately from \SCIP. The options are the following:
 *      - <code>clp</code>: COIN-OR Clp LP-solver
 *      - <code>cpx</code>: CPLEX LP-solver
 *      - <code>grb</code>: Gurobi LP-solver (interface is in beta stage)
 *      - <code>msk</code>: Mosek LP-solver
 *      - <code>qso</code>: QSopt LP-solver
 *      - <code>spx</code>: old SoPlex LP-solver (for versions < 2)
 *      - <code>spx2</code>: new SoPlex LP-solver (default) (from version 2)
 *      - <code>xprs</code>: XPress LP-solver
 *      - <code>none</code>: no LP-solver (you should set the parameter \<lp/solvefreq\> to \<-1\> to avoid solving LPs)
 *
 * - <code>LPSOPT=\<dbg|opt|opt-gccold\></code> Chooses the debug or optimized version (or old GCC optimized) version of
 *   the LP-solver (currently only available for SoPlex and CLP).
 *
 * - <code>ZIMPL=\<true|false\></code> Turns direct support of ZIMPL in \SCIP on (default) or off, respectively.\n
 *   If the ZIMPL-support is disabled, the GMP-library is no longer needed for \SCIP and therefore not linked to \SCIP.
 *
 * - <code>ZIMPLOPT=\<dbg|opt|opt-gccold\></code> Chooses the debug or optimized (default) (or old GCC optimized)
 *   version of ZIMPL, if ZIMPL support is enabled.
 *
 * - <code>READLINE=\<true|false\></code> Turns support via the readline library on (default) or off, respectively.
 *
 * - <code>FILTERSQP=\<true|false\></code> Enable or disable (default) FilterSQP interface.
 *
 * - <code>IPOPT=\<true|false\></code> Enable or disable (default) IPOPT interface (needs IPOPT >= 3.12).
 *
 * - <code>WORHP=\<true|false\></code> Enable or disable (default) WORHP interface (needs WORHP >= 2.0).
 *
 * - <code>EXPRINT=\<cppad|none\></code> Use CppAD as expressions interpreter (default) or no expressions interpreter.
 *
 * - <code>NOBLKBUFMEM=\<true|false\></code> Turns the internal \SCIP block and buffer memory off or on (default).
 *   This way the code can be checked by valgrind or similar tools. (The individual options <code>NOBLKMEM=\<true|false\></code>
 *   and <code>NOBUFMEM=\<true|false\></code> to turn off the \SCIP block and buffer memory, respectively, exist as well).
 *
 * - <code>TPI=\<tny|omp|none\></code> This determines the threading library that is used for the concurrent solver.
 *   The options are the following:
 *      - <code>none</code>: use no threading library and therefore disable the concurrent solver feature
 *      - <code>tny</code>: use the tinycthread's library which is bundled with SCIP. This
 *                          is a wrapper around the plattform specific threading library ad should work
 *                          for Linux, Mac OS X and Windows.
 *      - <code>omp</code>: use the OpenMP. This will not work with microsoft compilers, since they do not support
 *                          the required OpenMP version.
 *
 * - <code>SYM=\<bliss|none\></code> This determines the graph automorphism code used to compute symmetries of mixed
 *   integer programs if symmetry handling is enabled. The options are the following:
 *      - <code>none</code>: do not use a graph automorphism code, i.e., symmetries cannot be handled
 *      - <code>bliss</code>: use bliss to compute symmetries.
 *
 * You can use other compilers - depending on the system:
 *
 * - <code>COMP=<clang|gnu|intel></code> Use Clang, Gnu (default) or Intel compiler.
 *
 * There are additional parameters for Linux/Gnu compilers:
 *
 * - <code>SHARED=\<true\></code> generates a shared object of the \SCIP libraries.  (The binary uses these shared
 *   libraries as well.)
 * - <code>OPT=prf</code> generates a profiling version of \SCIP providing a detailed statistic of the time usage of
 *   every method of \SCIP.
 *
 * There is the possibility to watch the compilation more precisely:
 *
 * - <code>VERBOSE=\<true|false\></code> Turns the extensive output on or off (default).
 *
 * The \SCIP makefile supports several targets (used via <code>make ... "target"</code>):
 *
 * - <code>all</code> (or no target) Build \SCIP library and binary.
 * - <code>links</code> Reconfigures the links in the "lib" directory.
 * - <code>doc</code> Creates documentation in the "doc" directory.
 * - <code>clean</code> Removes all object files.
 * - <code>depend</code> Updates dependencies files. This is only needed if you add checks for preprocessor-defines `WITH_*` or SCIP_THREADSAFE in source files.
 * - <code>check</code> or <code>test</code> Runs the check script, see \ref TEST.
 * - <code>lint</code> Statically checks the code via flexelint. The call produces the file <code>lint.out</code>
 *   which contains all the detected warnings.
 * - <code>tags</code> Generates tags which can be used in the editor <b>emacs</b> and <b>xemacs</b>.

 * The \SCIP makefiles are structured as follows.
 *
 * - <code>Makefile</code> This is the basic makefile in the \SCIP root directory. It loads
 *   additional makefile information depending on the parameters set.
 * - <code>make/make.project</code> This file contains definitions that are useful for all codes
 *   that use \SCIP, for instance, the example.
 * - <code>make.\<sys\>.\<machine\>.\<compiler\>.\<dbg|opt|prf|opt-gccold\></code> These file contain system/compiler specific
 *   definitions. If you have an unsupported compiler, you can copy one of these and modify it
 *   accordingly.
 *
 * If your platform or compiler is not supported by \SCIP you might try and copy one of the existing
 * makefiles in the <code>make</code> directory and modify it. If you succeed, we are always
 * interested in including more Makefiles into the system.
 *
 *
 * @section EXAMPLE1 Example 1 (defaults: SoPlex, with ZIMPL support):
 *
 * Typing <code>make</code> uses SoPlex as LP solver and includes support for the modeling language ZIMPL. You will be asked the
 * following questions on the first call to "make" (example answers are already given):
 *
 * \verbinclude makeexamples/example1.txt
 *
 * @section EXAMPLE2 Example 2 (CPLEX, with no ZIMPL support):
 *
 * Typing <code>make LPS=cpx ZIMPL=false</code>  uses CPLEX as LP solver. You will be asked the following questions on
 * the first call to "make" (example answers are already given):
 *
 * \verbinclude makeexamples/example2.txt
 *
 *
 * @section COMPILERPROBLEMS Compilation problems:
 *
 * - If the soft-link query script does not work on your machine, read step 2 in the INSTALL file for
 * instructions on manually creating the soft-links.
 *
 * - If you get an error message of the type\n
 * <code>make: *** No rule to make target `lib/???', needed by `obj/O.linux.x86.gnu.opt/lib/scip/???.o'.  Stop.</code>\n
 * the corresponding soft-link was not created or points to a wrong location.  Check the soft-link targets in the "lib/"
 * subdirectory. Try to delete all soft-links from the "lib/" directory\n and call "make links" to generate them
 * again. If this still fails, read step 2 for instructions on manually\n creating the soft-links.
 *
 * - If you get an error message of the type\n
 * <code>make: *** No rule to make target `make/make.?.?.?.?.?'.  Stop.</code>,\n
 * the corresponding machine dependent makefile for your architecture and compiler is missing.\n Create one of the given
 * name in the "make/" subdirectory. You may take\n "make/make.linux.x86.gnu.opt" or any other file in the make
 * subdirectory as example.\n
 *
 * - The readline library seems to differ slightly on different OS distributions. Some versions do
 * not support the <code>remove_history()</code> call.  In this case, you have to either add
 * <code>-DNO_REMOVE_HISTORY</code> to the FLAGS in the appropriate "make/make.*" file, or to
 * compile with <code>make USRFLAGS=-DNO_REMOVE_HISTORY</code>.  Make sure, the file
 * "src/scip/dialog.c" is recompiled.  If this doesn't work either, disable the readline library
 * with <code>make READLINE=false</code>.
 *
 * - On some systems, the <code>sigaction()</code> method is not available. In this case, you have
 * to either add <code>-DNO_SIGACTION</code> to the FLAGS in the appropriate "make/make.*" file, or
 * to compile with <code>make USRFLAGS=-DNO_SIGACTION</code>.  Make sure, the file
 * "src/scip/interrupt.c" is recompiled.
 *
 * - On some systems, the <code>rand_r()</code> method is not available.  In this case, you have to either add
 * <code>-DNO_RAND_R</code> to the FLAGS in the appropriate "make/make.*" file, or to compile with
 * <code>make USRFLAGS=-DNO_RAND_R</code>.  Make sure, the file "src/scip/misc.c" is recompiled.
 *
 * - On some systems, the <code>strtok_r()</code> method is not available.  In this case, you have
 * to either add <code>-DNO_STRTOK_R</code> to the FLAGS in the appropriate make/make.* file, or to
 * compile with <code>make USRFLAGS=-DNO_STRTOK_R</code>.  Make sure, the file "src/scip/misc.c" is
 * recompiled.
 *
 * - On some systems, the <code>strerror_r()</code> method is not available.  In this case, you have
 * to either add <code>-DNO_STRERROR_R</code> to the FLAGS in the appropriate "make/make.*" file, or
 * to compile with <code>make USRFLAGS=-DNO_STRERROR_R</code>.  Make sure, the file
 * "src/scip/misc.c" is recompiled.
 *
 * - On some systems, the option [-e] is not available for the read command.  You have to compile with READ=read.
 *
 * - If you encounter other compiler or linker errors, you should recompile with <code>make
 * VERBOSE=true ...</code> in order to get the full compiler invocation. This might help to fix the
 * corresponding machine dependent makefile in the make subdirectory.
 *
 * @section WINDOWS Remarks on Installing under Windows using MinGW
 *
 * To build your own windows binaries under windows we recommend using the MinGW-Compiler with MSYS
 * from <a href="http://www.mingw.org">www.mingw.org</a> .
 *
 * First install MSYS, then MinGW to the mingw folder inside the msys folder.
 * Now you need to install the following packages to the mingw folder:
 * - zlib (or use ZLIB=false)
 * - pcre (here suffices the pcre7.0-lib.zip (or equivalent) to be extracted into the mingw-folder)
 *
 * After calling <code>make clean</code> in the ZIMPL folder you will also need flex and bison to
 * remake ZIMPL. We recommend NOT to use <code>"make clean"</code> inside the ZIMPL-folder if you do
 * not have these packages installed.
 *
 * You can download these additional packages from <a href="http://gnuwin32.sourceforge.net/packages.html">here</a>
 * or compile the source on your own from their homepages.
 *
 * Second you need to copy the file <code>sh.exe</code> to <code>bash.exe</code> otherwise various
 * scripts (including makefiles) will not work.  Normally <code>unistd.h</code> covers also the
 * getopt-options, but for mingw you need to add the entry <code>\#include <getopt.h></code> into
 * "/mingw/include/unistd.h" after the other include-entries (if not present).
 *
 * Finally, there is one package you need to compile if you want to use ZIMPL and ZIMPL-support in
 * \SCIP (otherwise use <code>ZIMPL=false</code> as parameter with the make-call): the
 * <code>gmplib</code> from <a href="http://www.gmplib.org">gmplib.org</a>. The command
 * <code>./configure --prefix=/mingw ; make ; make install</code> should succeed without problems
 * and installs the gmplib to the mingw folder.
 *
 * Now <code>make READLINE=false</code> should be compiling without errors.  Please note that we
 * do NOT support creating the doxygen documentation and readline-usage under windows.
 *
 *
 * @section RUN How to run SCIP after a successful compilation
 *
 * To run the program, enter <code>bin/scip</code> for the last compiled version. If you have more than one compiled
 * binary (i. e., one in debug and one in optimized mode) and wish to specify the binary, type
 * <code>bin/scip.\$(OSTYPE).\$(ARCH).\$(COMP).\$(OPT).\$(LPS)</code>
 * (e.g. <code>bin/scip.linux.x86_64.gnu.opt.spx</code>).
 *
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page START How to start a new project
 *
 * Once you succeeded installing \SCIP together with an LP-solver on your system,
 * you have a powerful tool for solving MIPs, MIQCPs,
 * MINLPs, etc... at hand. \SCIP can also be customized to the type of problems you
 * are working on by additional plugins.
 * Instructions on how to write a new plugin and include it in \SCIP can be found in the corresponding
 * \ref HOWTOADD "How to add ... pages".
 *
 * \SCIP can also be used for writing your own branch-and-cut or branch-and-cut-and-price code. \SCIP already
 * provides a number of existing code examples which we suggest as both reference and starting point
 * for these kinds of projects.
 * Below, you find some hints of how to start such a project.
 *
 * @section START_CHOICE Choose an example project
 *
 *  The example should be chosen depending on the programming language (<b>C</b> or <b>C++</b>) and the purpose
 *   (<b>branch-and-cut</b> or <b>branch-and-cut-and-price</b>) of your project.
 *   <br>
 *   We suggest the use of one of the following examples:
 *     - The \ref VRP_MAIN "Vehicle Routing Problem Example" is a <b>branch-and-cut-and-price</b> (column generation)-code
 *       in <b>C++</b>.
 *     - The \ref BINPACKING_MAIN "Binpacking Example"
 *       and the \ref COLORING_MAIN "Coloring application" are
 *       <b>branch-and-cut-and-price</b> (column generation)-codes in <b>C</b>.
 *     - The \ref TSP_MAIN "TSP example"
 *        is a <b>branch-and-cut</b>-code in <b>C++</b>.
 *     - The \ref LOP_MAIN "LOP example"
 *       is a <b>branch-and-cut</b>-code in <b>C</b>.
 *
 * More examples can be found in the \ref EXAMPLES "list of Examples".
 *
 * - Copy one of the examples in the <code>examples</code> directory (in the \SCIP root
 *   directory). For instance, type
 *   \verbatim
 cp -r examples/Binpacking/ ../SCIPProject/ ; cd ../SCIPProject
     \endverbatim
 *   from the \SCIP root directory for copying the content of the <code>Binpacking</code>-example into a fresh
 *   directory named SCIPProject in the parent directory of the \SCIP root directory and jumping to
 *   the new SCIPProject directory rightafter.
 *
 * @section START_CMAKE Building with CMake
 *
 * It is recommended for all new users to use the CMake build system configuration, if available on their platform.
 *
 * - Open the <code>CMakeLists</code> (some examples projects have a subdirectory "check" for testing) via
 *    \verbatim
 kate CMakeLists.txt & kate check/CMakeLists.txt # if check/CMakeLists.txt is available
     \endverbatim
 *    and replace all instances of the copied project's name (e.g. <code>binpacking</code>) with your project name.
 * - Create a new subdirectory, jump to the new directory and use cmake specifying your \SCIP directory. For instance, type
 *    \verbatim
 mkdir Release ; cd Release ; cmake .. -DSCIP_DIR=../scip/Release
     \endverbatim
 * and compile using the <code>make</code> command. For the CMake equivalents of all the flags that can be used in \SCIP, see \ref CMAKE.
 *
 *
 * @section START_MAKE Building with the Makefile system
 *
 * If CMake should be unavailable on your targeted platform, try the classic Makefile system of SCIP.
 *
 * - Open the <code>Makefile</code>  via
 *    \verbatim
 kate Makefile
     \endverbatim
 *    and edit the following variables at the top to have a compilable code:
 *
 *    - specify a correct path to the \SCIP root (<code>SCIPDIR</code>)
 *    - rename the targets name (<code>MAINNAME</code>)
 *    - adjust the source file names (<code>MAINOBJ</code>).
 *    .
 * - Once you have edited the makefile, you can use all the flags that can be used in \SCIP to
 *   compile your code, see \ref MAKE.
 *
 */

/**@page EXAMPLES Example projects
 *
 *  \SCIP contains several examples that demonstrate its usage. They are contained in the &quot;examples&quot; directory
 *  in the source code distribution.
 *
 * <table>
 *  <tr>
 *  <td colspan="2">
 *  <b>
 *  Callable library
 *  </b>
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage CALLABLELIBRARY_MAIN "Callable Library Example"
 *  </td>
 *  <td>
 *  An example showing how to setup constraints (esp. nonlinear ones) when using \SCIP as callable library.
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage MIPSOLVER_MAIN "MIP solver"
 *  </td>
 *  <td>
 *  A minimal implementation for using \SCIP included into another source code
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage QUEENS_MAIN "The n-Queens Problem"
 *  </td>
 *  <td>
 *  Using SCIP's callable library for solving the n-queens problem.
 *  </td>
 *  </tr>
 *  <tr>
 *  <td colspan="2">
 *  <b>
 *  Extending SCIP by custom plugins
 *  </b>
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage EVENTHDLR_MAIN "Event handler"
 *  </td>
 *  <td>
 *  A small example illustrating the use of an event handler.
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage GMI_MAIN "Gomory mixed integer cut example"
 *  </td>
 *  <td>
 *  An example about Gomory mixed-integer cuts.
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage RELAXATOR_MAIN "Relaxator example"
 *  </td>
 *  <td>
 *  An example about using custom relaxators.
 *  </td>
 *  </tr>
 *  <tr>
 *  <td colspan="2">
 *  <b>
 *  Branch-and-cut
 *  </b>
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage SUDOKU_MAIN "Sudoku example"
 *  </td>
 *  <td>
 *  An example solving sudokus.
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage LOP_MAIN "Linear Ordering"
 *  </td>
 *  <td>
 *  An example for implementing a constraint handler.
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage TSP_MAIN "The TSP example"
 *  </td>
 *  <td>
 *  A short implementations of a constraint handler, two easy combinatorial heuristics, a file reader, etc. which
 *  demonstrate the usage of \SCIP as a branch-and-cut-framework for solving traveling salesman problem instances.
 *  </td>
 *  </tr>
 *  <tr>
 *  <td colspan="2">
 *  <b>
 *  Branch-and-price
 *  </b>
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage BINPACKING_MAIN "Binpacking"
 *  </td>
 *  <td>
 *  An implementation of the column generation approach for the binpacking problem. It includes a customized reader,
 *  Ryan/Foster branching rule, (global) problem data, variable data, and constraint handler.
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage VRP_MAIN "Vehicle Routing"
 *  </td>
 *  <td>
 *  A solver for a simple capacity-constrained vehicle routing problem, which is based on pricing tours via a dynamic
 *  programming algorithm.
 *  </td>
 *  </tr>
 *  <tr>
 *  <td colspan="2">
 *  <b>
 *  Benders' decomposition
 *  </b>
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage SCFLP_MAIN "Stochastic capacitated facility location problem"
 *  </td>
 *  <td>
 *  A stochastic programming problem that demonstrates the use of the Benders' decomposition framework within SCIP.
 *  </td>
 *  </tr>
 *  </table>
 */

/** @page APPLICATIONS Application projects
 *
 *  There are several extensions of \SCIP for particular applications included in the release. They are contained in the &quot;applications&quot; directory
 *  in the source code distribution.
 *
 *  <table>
 *  <tr>
 *  <td>
 *  @subpage COLORING_MAIN
 *  </td>
 *  <td>
 *  An implementation of the column generation approach for graph coloring of Mehrotra and Trick.
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage CYCLECLUSTERING_MAIN
 *  </td>
 *  <td>
 *  Branch-and-cut implementation of a graph partitioning problem used for Markov state models.
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage MINIISC_MAIN
 *  </td>
 *  <td>
 *  A solver that computes irreducible infeasible subsystems using Benders decomposition
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage POLYSCIP_MAIN
 *  </td>
 *  <td>
 *  A solver for multi-objective optimization problems.
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage RINGPACKING_MAIN "Ringpacking"
 *  </td>
 *  <td>
 *  An implementation of the column generation approach for the Ringpacking Problem. It includes a customized reader,
 *  (global) problem data, variable data, and constraint handler.
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage SCHEDULER_MAIN
 *  </td>
 *  <td>
 *  A solver for scheduling problems.
 *  </td>
 *  </tr>
 *  <tr>
 *  <td>
 *  @subpage STP_MAIN
 *  </td>
 *  <td>
 *  A solver for Steiner Tree Problems in graphs, based on a branch-and-cut approach.
 *  </td>
 *  </tr>
 *  </table>
 *
 */


/**@page SHELL Tutorial: the interactive shell
 *
 * If you are using \SCIP as a black box solver, here you will find some tips and tricks what you can do.
 *
 * @section TUTORIAL_OPTIMIZE Read and optimize a problem instance
 *
 * First of all, we need a \SCIP binary and an example problem file to work with. Therefore, you can either download the
 * \SCIP standard distribution (which includes problem files) and compile it on your own or you can download a
 * precompiled binary and an example problem separately. \SCIP can read files in LP, MPS, ZPL, WBO, FZN, PIP, OSiL, and
 * other formats (see \ref FILEREADERS).
 *
 * If you want to download the source code of the \SCIP standard distribution, we recommend to go to the <a
 * href="http://scipopt.org/#download">SCIP download section</a>, download the latest release (version 4.0.0 as
 * of this writing), inflate the tarball (e.g., with "tar xzf scipoptsuite-[version].tgz"), and follow the instructions
 * in the INSTALL file. The instance stein27, which will serve as an example in this tutorial, can be found under
 * scipoptsuite-[version]/scip-[version]/check/instances/MIP/stein27.fzn.
 *
 * If you want to download a precompiled binary, go to the <a href="http://scipopt.org/#download">SCIP download
 * section</a> and download an appropriate binary for your operating system. The \SCIP source code distribution already comes with
 * the example instance used throughout this tutorial. To follow this tutorial with a precompiled binary, we recommend downloading the instance
 * <a href="http://miplib2010.zib.de/miplib3/miplib3/stein27.mps.gz">stein27</a> from
 * the <a href="http://miplib2010.zib.de/miplib3/miplib.html">MIPLIB 3.0</a> homepage.
 *
 * Now start your binary, without any arguments. This opens the interactive shell, which should look somehow like this:
 *
 * @snippet shelltutorial/shelltutorialannotated.tmp SnippetVersion
 *
 * First of all "help" shows you a list of all available shell commands. Brackets indicate a submenu with further options.
 *
 * @snippet shelltutorial/shelltutorialannotated.tmp SnippetHelp
 *
 * Okay, let's solve the example instance... use "read check/instances/MIP/stein27.fzn" to parse the instance file, "optimize" to solve it and "display
 * solution" to show the nonzero variables of the best found solution.
 *
 * @snippet shelltutorial/shelltutorialannotated.tmp SnippetOpt1
 *
 * What do we see here? After "optimize", SCIP first goes into presolving. Not much is happening for this instance, just
 * the linear constraints get upgraded to more specific types. Each round of presolving will be displayed in a single
 * line, with a short summary at the end. Then, we see the actual solving process. The table output of the branch-and-cut
 * solving process is very detailed during the root node. Afterwards, a new line is displayed every 100th node.
 * Furthermore, every new incumbent solution triggers a new table row, starting with a character to indicate the
 * heuristic that found the solution. Which letter represents which heuristic can be seen with the
 * "display heuristics" command, see \ref TUTORIAL_STATISTICS for an example.
 *
 * After some lines the root node processing is finished. From now on, we will see an output line every hundredth node or
 * whenever a new incumbent is found. After some more nodes, the "dualbound" starts
 * moving, too. At one point, both will be the same, and the solving process terminates, showing us some wrap-up
 * information.
 *
 * The exact performance may of course vary among different architectures and operating systems. Do not be worried if
 * your installation needs more or less time or nodes to solve. Also, this instance has more than 2000 different optimal
 * solutions. The optimal objective value always has to be 18, but the solution vector may differ. If you are interested
 * in this behavior, which is called "performance variability", you may have a look at the MIPLIB2010 paper.
 *
 * @section TUTORIAL_FILEIO Writing problems and solutions to a file

 * \SCIP can also write information to files. E.g., we could store the incumbent solution to a file, or output the
 * problem instance in another file format (the LP format is much more human readable than the MPS format, for example).
 *
 * @snippet shelltutorial/shelltutorialannotated.tmp SnippetWriteSolutions
 *
 * Passing starting solutions can increase the solving performance so that \SCIP does not need to construct an initial feasible solution
 * by itself. After reading the problem instance, use the "read" command again, this time with a file containing solution information.
 * Solutions can be specified in a raw or xml-format and must have the file extension ".sol", see the documentation of the
 * \ref reader_sol.h "solution reader of SCIP" for further information.
 *
 * Customized settings are not written or read with the "write" and "read" commands, but with the three commands
 *
 * @snippet shelltutorial/shelltutorialannotated.tmp SnippetSaveSettingsOverview
 *
 * See the section on parameters \ref TUTORIAL_PARAMETERS for more information.
 *
 * @section TUTORIAL_STATISTICS Displaying detailed solving statistics
 *
 * We might want to have some more information now. Which of the heuristics found solutions? Which plugins
 * were called during the solutions process and how much time did they spend?
 * Information on certain plugin types (e.g., heuristics, branching rules, separators) is displayed via
 * "display <plugin-type>", information on the solution process via "display statistics", and "display problem"
 * shows the current instance.
 *
 * @snippet shelltutorial/shelltutorialannotated.tmp SnippetDisplayStatistics
 *
 * The statistics obtained via "display statistics" are quite comprehensive,
 * thus, we just explain a few lines here. Information is grouped by the plugin type. For the primal heuristics,
 * the execution time in seconds is shown as well as the number of calls to the heuristic, and its success regarding
 * the number of (best) solutions found by that heuristic. Appropriate statistics are also shown for presolvers, constraint handlers,
 * separators, propagators, the search tree, etc. User-written plugins will appear automatically in these statistics,
 * after they were included into \SCIP.
 *
 * @section TUTORIAL_PARAMETERS Changing parameters from the interactive shell
 *
 * Now, we can start playing around with parameters. The primal heuristics Rounding and shifting seem to be quite successful on this instance,
 * wondering what happens if we disable them? Or what happens, if we are even more rigorous and disable all heuristics?
 * Or if we do the opposite and use aggressive heuristics?
 *
 * @snippet shelltutorial/shelltutorialannotated.tmp SnippetSetSettings
 *
 * We can navigate through the menus step-by-step and get a list of available options and submenus. Therefore, we select
 * "set" to change settings, "heuristics" to change settings of primal heuristics, and "shifting" for that particular
 * heuristic. Then we see a list of parameters (and yet another submenu for advanced parameters), and disable this
 * heuristic by setting its calling frequency to -1. If we already know the path to a certain setting, we can directly
 * type it (as for the rounding heuristic in the above example). Note that we do not have to use the full names, but we
 * may use short versions, as long as they are unique.
 *
 * To solve a problem a second time, we have to read it in again before starting the optimization process.
 *
 * @snippet shelltutorial/shelltutorialannotated.tmp SnippetOpt2
 *
 * Okay, what happened here? First, we reset all parameters to their default values, using "set default". Next, we
 * loaded some meta-parameter settings (also see <a href="FAQ.php#howtochangebehaviour">the FAQ</a>), to apply primal heuristics
 * more aggressively. \SCIP shows us, which single parameters it changed therefore. Additionally, for pedagogical purposes,
 * we set the node limit to 200. Now, the optimal solution is already found at the root node, by a heuristic which is
 * deactivated by default.  Then, after node 200, the user defined node limit is reached which interrupts the solving
 * process, We see that now in the short status report, primal and dual bound are different, thus, the problem is not solved
 * yet.  Nevertheless, we could access statistics, see the current incumbent solution, change parameters and so on.
 * Entering "optimize" we continue the solving process from the point on at which it has been interrupted.
 *
 * Once you found a non-default parameter setting that you wish to save and use in the future, use either the command
 *
 * @snippet shelltutorial/shelltutorialannotated.tmp SnippetSaveSettingsFull
 *
 * to save <b>all</b> parameter values to the specified file, or
 *
 * @snippet shelltutorial/shelltutorialannotated.tmp SnippetSaveSettingsDiff
 *
 * in order to save only the nondefault parameters. The latter has several advantages, you can, e.g., combine parameter
 * settings from multiple settings files stored by the latter command, as long as they only affect mutually exclusive
 * parameter values.
 *
 * For loading a previously stored settings file, use the "load" command:
 *
 * @snippet shelltutorial/shelltutorialannotated.tmp SnippetLoadSettings
 *
 * Special attention should be drawn to the reserved settings file name "scip.set"; whenever the \SCIP interactive shell
 * is started from a working directory that contains a settings file with the name "scip.set", it will be automatically
 * replace the default settings.
 *
 * For using special settings for automated tests as described in \ref TEST, save your custom settings in a subdirectory
 * "SCIP_HOME/settings".
 *
 *
 * We hope this tutorial gave you an overview of what is possible using the \SCIP interactive shell. Please also read our
 * \ref FAQ, in particular the section "Using SCIP as a standalone MIP/MINLP-Solver".
 *
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page DOC How to search the documentation for interface methods
 *
 * If you are looking for a method in order to perform a specific task, the public \ref PUBLICAPI "SCIP C-API" is the place to look.
 * - It contains interface methods for all SCIP structs, both in the solver core or in one of the plugins.
 * - Plugins are mostly independent from each other, so to use them it is usually enough to browse the \ref PUBLICCOREAPI "Core API".
 * - If you want to add your own plugins, see the \ref HOWTOADD pages for exhaustive information for each plugin type.
 * - If you are learning SCIP with a concrete project in mind, looking at the available \ref EXAMPLES page may help you
 *   getting started.
 * - See also \ref START "How to start a new project"
 *
 * Header file names of SCIP obey a consistent naming scheme: Type definitions and related objects such as enums are found in headers starting with "type_",
 * such as \ref type_var.h , which contains enums and type definitions related to \ref PublicVariableMethods "SCIP problem variables".
 * Definitions of the actual structs can be found in separate header files starting with "struct_".
 * All method definitions of the public SCIP API are split across header files starting with "pub_" such as \ref pub_cons.h
 * or headers starting with "scip_" such as \ref scip_cons.h .
 * The latter headers starting with "scip_" contain more complex methods, which always receive a scip pointer as first argument.
 * Those methods may affect several individual components controlled by SCIP. Such a method is SCIPbranchVar(), which
 * affects the search tree, which is controlled by SCIP itself and not meant to be accessed by user plugins.
 *
 * It should be sufficient to include scip/scip.h and scip/scipdefplugins.h for having all
 * needed functionality available in a project.
 *
 * If, for example, you are looking for information on how to create a problem instance, here are some steps you can take:
 *
 * 1. Browse the SCIP Core API and follow the path \ref PUBLICAPI > \ref PUBLICCOREAPI > \ref PublicProblemMethods > \ref GlobalProblemMethods > SCIPcreateProb()
 * 2. Here you can find information on the function's return value, preconditions, postconditions, parameters, as well as related functions.
 * 3. If you are unsure of how to use some of the parameters, it is worth looking for a basic version of the function.
 *   This and other related functions may be found by either browsing neighboring functions and groups in the navigation tree to the left, or in the
 *   'References' and 'Referenced by' section of the function documentation. In this case, you can find `SCIPcreateProbBasic()`.
 *
 * The opposite case is that you already know the name of a function as, e.g., SCIPbranchVar().
 *
 * 1. Type the name of the function into the search bar to find the function documentation.
 * 2. In addition, you can find related methods by browsing the neighboring functions of the same group.
 * 3. In this example, you may now learn about SCIPgetNLPBranchCands() to query all integer
 *    variables with fractional LP solution value, which are good candidates for classical branching on variables.
 *
 * Note that the \ref INTERNALAPI "private SCIP API" contains more complex functions and data structures that fill specialized roles and
 * is only for developers.
 * Those functions are **not** exported to the library and are therefore **not available in user projects** using the \ref PUBLICAPI "public SCIP API".
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page CONS How to add constraint handlers
 *
 * A constraint handler defines the semantics and the algorithms to process constraints of a certain class.  A single
 * constraint handler is responsible for all constraints belonging to its constraint class.  For example, there is
 * one \ref cons_knapsack.h "knapsack constraint handler" that ensures solutions are only accepted if they satisfy all
 * knapsack constraints in the model. \n A complete list of all constraint handlers contained in this release can be
 * found \ref CONSHDLRS "here".
 *
 * We now explain how users can add their own constraint handlers.
 * For an example, look into the subtour constraint handler (examples/TSP/src/ConshdlrSubtour.cpp) of the
 * \ref TSP_MAIN project.
 * The example is written in C++ and uses the C++ wrapper classes.
 * However, we will explain the implementation of a constraint handler using the C interface.
 * It is very easy to transfer the C explanation to C++; whenever a method should be implemented using the
 * SCIP_DECL_CONS... notion, reimplement the corresponding virtual member function of the abstract scip::ObjConshdlr
 * base class.
 *
 * Additional documentation for the callback methods of a constraint handler can be found in the file
 * type_cons.h.
 *
 * Here is what you have to do (assuming your constraint handler should be named "subtour"):
 * -# Copy the template files src/scip/cons_xyz.c and src/scip/cons_xyz.h into files "cons_subtour.c"
 *    and "cons_subtour.h".
 *     \n
 *    Make sure to <b>adjust your Makefile</b> such that these files are compiled and linked to your project.
 * -# Use SCIPincludeConsSubtour() in order to include the constraint handler into your SCIP instance,
 *    e.g., in the main file of your project (see, e.g., src/cppmain.cpp in the TSP example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "subtour".
 * -# Adjust the \ref CONS_PROPERTIES "properties of the constraint handler".
 * -# Define the \ref CONS_DATA "constraint data and the constraint handler data". This is optional.
 * -# Implement the \ref CONS_INTERFACE "interface methods".
 * -# Implement the \ref CONS_FUNDAMENTALCALLBACKS "fundamental callback methods".
 * -# Implement the \ref CONS_ADDITIONALCALLBACKS "additional callback methods". This is optional.
 *
 *
 * @section CONS_PROPERTIES Properties of a Constraint Handler
 *
 * At the top of the new file "cons_subtour.c" you can find the constraint handler properties.
 * These are given as compiler defines. Some of them are optional, as, e.g., separation-related properties,
 * which only have to be defined if the constraint handler supports the related callbacks.
 * In the C++ wrapper class, you have to provide the constraint handler properties by calling the constructor
 * of the abstract base class scip::ObjConshdlr from within your constructor (see the TSP example).
 * The properties you have to set have the following meaning:
 *
 * @subsection CONS_FUNDAMENTALPROPERTIES Fundamental Constraint Handler properties
 *
 * \par CONSHDLR_NAME: the name of the constraint handler.
 * This name is used in the interactive shell to address the constraint handler.
 * Additionally, if you are searching for a constraint handler with SCIPfindConshdlr(), this name is looked up.
 * Names have to be unique: no two constraint handlers may have the same name.
 *
 * \par CONSHDLR_DESC: the description of the constraint handler.
 * This string is printed as a description of the constraint handler in the interactive shell of SCIP.
 *
 * \par CONSHDLR_ENFOPRIORITY: the priority of the constraint handler for constraint enforcing.
 * Like the separation priority, the enforcement priorities define the order in which the different constraint handlers
 * are called in the constraint enforcement step of the subproblem processing.
 * The constraint enforcement is called after the price-and-cut loop is executed (in the case that the LP is solved
 * at the current subproblem).
 * \n
 * The integrality constraint handler has an enforcement priority of 0.
 * That means, if a constraint handler has negative enforcement priority, it only has to deal with integral solutions
 * in its enforcement methods, because for fractional solutions, the integrality constraint handler would have
 * created a branching, thereby aborting the enforcement step.
 * If you want to implement a constraint-depending branching rule (for example, SOS branching on special ordered
 * set constraints), you have to assign a positive enforcement priority to your constraint handler.
 * In this case, you have to be able to deal with fractional solutions.
 * \n
 * See \ref CONSENFOLP and \ref CONSENFOPS for further details of the separation callback.
 *
 * \par CONSHDLR_CHECKPRIORITY: the priority of the constraint handler for checking feasibility.
 * Like the separation priority, the checking priorities define the order in which the different constraint handlers
 * are called to check the feasibility of a given primal solution candidate.
 * The integrality constraint handler has a checking priority of 0.
 * That means, constraint handlers with negative checking priorities only have to deal with integral solutions.
 *
 * \par CONSHDLR_EAGERFREQ: the default frequency for using all instead of only the useful constraints in separation, propagation and enforcement.
 * If \em constraint \em aging is activated, some constraints that were not useful in the past for propagation or
 * separation are marked to be \em obsolete.
 * Usually, the obsolete constraints are not presented to the separation and propagation methods of the constraint
 * handlers, such that the constraint handlers only process the non-obsolete constraints.
 * However, every n'th call, with n being the EAGERFREQ of the constraint handler, all constraints are presented to the
 * separation and propagation methods of the constraint handler.
 * This gives obsolete constraints the chance of becoming non-obsolete again.
 * \n
 * If the eager evaluation frequency is set to -1, obsolete constraints are never presented to the separation and
 * propagation methods.
 * A frequency of 0 means, that obsolete constraints are only used in the first call of each method.
 *
 * \par CONSHDLR_NEEDSCONS: indicates whether the constraint handler should be skipped, if no constraints are available.
 * Usually, a constraint handler is only executed if there are constraints of its corresponding class in the model.
 * For those constraint handlers, the NEEDSCONS flag should be set to TRUE.
 * However, some constraint handlers must be called without having a constraint of the class in the model, because
 * the constraint is only implicitly available.
 * For example, the integrality constraint handler has the NEEDSCONS flag set to FALSE, because there is no explicit
 * integrality constraint in the model.
 * The integrality conditions are attached to the variables, and the integrality constraint handler has to check
 * all variables that are marked to be integer for integral values.
 *
 * @subsection CONS_ADDITIONALPROPERTIES Optional Constraint Handler properties
 *
 * The following properties are optional and only need to be defined if the constraint handlers support
 * separation, presolving, propagation, and/or upgrade functionality.
 *
 * \par LINCONSUPGD_PRIORITY: priority of the constraint handler for upgrading of linear constraints
 * This property is only needed if a certain linear constraint can be upgraded to a more specific one. In one of
 * the first presolving rounds SCIP tries to upgrade linear constraints to more specialized constraints, such as
 * knapsack constraints. The upgrading calls are processed in the order of decreasing priority.
 *
 * \par NONLINCONSUPGD_PRIORITY: priority of the constraint handler for upgrading of nonlinear constraints
 * This property has the same effect as the LINCONSUPGD_PRIORITY parameter, see above, and should be set whenever
 * an upgrade functionality from a general nonlinear constraint to the more specific one is defined.
 *
 * \par CONSHDLR_SEPAFREQ: the default frequency for separating cuts.
 * The separation frequency defines the depth levels at which the constraint handler's separation methods \ref CONSSEPALP
 * and \ref CONSSEPASOL are called.
 * For example, a separation frequency of 7 means, that the separation callback is executed for subproblems that are
 * in depth 0, 7, 14, ... of the branching tree.
 * A separation frequency of 0 means, that the separation method is only called at the root node.
 * A separation frequency of -1 disables the separation method of the constraint handler.
 * \n
 * The separation frequency can be adjusted by the user.
 * This property of the constraint handler only defines the default value of the frequency.
 * If you want to have a more flexible control of when to execute the separation algorithm, you have to assign
 * a separation frequency of 1 and implement a check at the beginning of your separation algorithm whether you really
 * want to execute the separator or not.
 * If you do not want to execute the method, set the result code to SCIP_DIDNOTRUN.
 *
 * \par CONSHDLR_SEPAPRIORITY: the priority of the constraint handler for separation. (optional: to be set only if the constraint handler supports separation)
 * In each separation round during the price-and-cut loop of the subproblem processing or during the separation loop
 * of the primal solution separation, the separators and separation methods of the constraint handlers are called in
 * a predefined order, which is given by the priorities of the separators and the separation priorities of the
 * constraint handlers.
 * First, the separators with non-negative priority are called in the order of decreasing priority.
 * Next, the separation methods of the different constraint handlers are called in the order of decreasing separation
 * priority.
 * Finally, the separators with negative priority are called in the order of decreasing priority.
 * \n
 * The separation priority of the constraint handler should be set according to the complexity of the cut separation
 * algorithm and the impact of the resulting cuts:
 * Constraint handlers that provide fast algorithms that usually have a high impact (i.e., cut off a large portion of
 * the LP relaxation) should have a high priority.
 * See \ref CONSSEPALP and \ref CONSSEPASOL for further details of the separation callbacks.
 *
 * \par CONSHDLR_DELAYSEPA: the default for whether the separation method should be delayed, if other separators found cuts.
 * If the constraint handler's separation method is marked to be delayed, it is only executed after no other separator
 * or constraint handler found a cut during the price-and-cut loop.
 * If the separation method of the constraint handler is very expensive, you may want to mark it to be delayed until all
 * cheap separation methods have been executed.
 *
 * \par CONSHDLR_PROPFREQ: the default frequency for propagating domains.
 * This default frequency has the same meaning as the CONSHDLR_SEPAFREQ with respect to the domain propagation
 * callback of the constraint handler.
 * A propagation frequency of 0 means that propagation is only applied in preprocessing and at the root node.
 * A propagation frequency of -1 disables the propagation method of the constraint handler.
 *
 * \par CONSHDLR_DELAYPROP: the default for whether the propagation method should be delayed, if other propagators found reductions.
 * This property is analogous to the DELAYSEPA flag, but deals with the propagation method of the constraint handler.
 *
 * \par CONSHDLR_PROP_TIMING: the propagation timing mask of the constraint handler.
 * SCIP calls the domain propagation routines at different places in the node processing loop.
 * This property indicates at which places the propagation routine of the constraint handler is called.
 * Possible values are defined in type_timing.h and can be concatenated, e.g., as in SCIP_PROPTIMING_ALWAYS.
 *
 * \par CONSHDLR_PRESOLTIMING: the timing of the constraint handler's presolving method (FAST, MEDIUM, or EXHAUSTIVE).
 * Every presolving round starts with the FAST presolving methods. MEDIUM presolvers are only called, if FAST presolvers did not find
 * enough reductions in this round so far, and EXHAUSTIVE presolving steps are only performed if all presolvers called before
 * in this round were unsuccessful.
 * Presolving methods should be assigned a timing based on how expensive they are, e.g., presolvers that provide fast algorithms that
 * usually have a high impact (i.e., remove lots of variables or tighten bounds of many variables) should have a timing FAST.
 * If a presolving method implements different algorithms of different complexity, it may also get multiple timings and check the timing
 * internally in the \ref CONSPRESOL callback to decide which algorithms to run.
 *
 * \par CONSHDLR_MAXPREROUNDS: the default maximal number of presolving rounds the constraint handler participates in.
 * The preprocessing is executed in rounds.
 * If enough changes have been applied to the model, an additional preprocessing round is performed.
 * The MAXPREROUNDS parameter of a constraint handler denotes the maximal number of preprocessing rounds the constraint
 * handler participates in.
 * A value of -1 means that there is no limit on the number of rounds.
 * A value of 0 means the preprocessing callback of the constraint handler is disabled.
 *
 *
 *
 * @section CONS_DATA Constraint Data and Constraint Handler Data
 *
 * Below the header "Data structures" you can find two structs called "struct SCIP_ConsData" and
 * "struct SCIP_ConshdlrData".
 * If you are using C++, you only need to define the "struct SCIP_ConsData".
 * The constraint handler data must be implemented as member variables of your constraint handler class.
 * \n
 * The constraint data are the information that is needed to define a single constraint of the constraint handler's
 * constraint class.
 * For example, the data of a knapsack constraint would consist of a list of variables, a list of weights, and
 * the capacity of the knapsack.
 * The data of a subtour constraint consists of the graph on which the problem is defined.
 * In the graph, each edge should be linked to the corresponding binary problem variable.
 * \n
 * The constraint handler data are additional variables, that belong to the constraint handler itself and which are
 * not specific to a single constraint.
 * For example, you can use these data to store parameters of the constraint handler or statistical information.
 * The constraint handler data are optional.
 * You can leave the struct empty.
 *
 *
 * @section CONS_INTERFACE Interface Methods
 *
 * At the bottom of "cons_subtour.c" you can find three interface methods, that also appear in "cons_subtour.h".
 * These are SCIPincludeConshdlrSubtour(), SCIPcreateConsSubtour(), and SCIPcreateConsSubtourBasic().
 * \n
 * The method SCIPincludeConshdlrSubtour() only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the constraint handler by calling the method
 * SCIPincludeConshdlr().
 * It is called by the user, if (s)he wants to include the constraint handler, i.e., if (s)he wants to make
 * the constraint handler available to the model, and looks like this:
 * \dontinclude src/scip/cons_knapsack.c
 *  -# If you are using constraint handler data, you have to <b>allocate the memory for the data</b> at this point.
 *     You also have to initialize the fields in struct SCIP_ConshdlrData afterwards.
 *
 *     \skip SCIP_RETCODE SCIPincludeConshdlrKnapsack(
 *     \until SCIPallocBlockMemory
 *
 *  -# Now, <b>SCIP gets notified</b> of the presence of the constraint handler together with its \ref CONS_FUNDAMENTALCALLBACKS "basic callbacks".
 *
 *     \skip SCIPincludeConshdlrBasic
 *     \until assert
 *
 *  -# All \ref CONS_ADDITIONALCALLBACKS "additional callbacks" are added via their setter functions.
 *
 *     \skip SCIPsetConshdlrCopy
 *     \until SCIPsetConshdlrExit
 *
 *  -# If the constraint handler is a specialization of a general linear or nonlinear constraint, we want to include an
 *     <b>automatic upgrading mechanism</b> by calling the interface method
 *
 *     \skip SCIPfindConshdlr
 *     \until SCIPincludeLinconsUpgrade
 *     or
 *     \code
 *     SCIP_CALL( SCIPincludeNonlinconsUpgrade(scip, nonlinconsUpgdSubtour, NULL, NONLINCONSUPGD_PRIORITY, TRUE, CONSHDLR_NAME) );
 *     \endcode
 *
 *     in the nonlinear case. See also cons_nonlinear.h for further information about the general upgrade procedure in the nonlinear case.
 *  -# You may also add <b>user parameters</b> for your constraint handler.
 * Some parameters which are important to play with are added to every constraint automatically, as, e.g.,
 * propagation or separation frequency.
 *     \skip SCIPaddIntParam
 *     \until DEFAULT_SEPACARDFREQ
 *     \skip SCIP_OKAY
 *     \until }
 *
 *
 * The methods SCIPcreateConsSubtour() and SCIPcreateConsSubtourBasic() are called to create a single constraint of the constraint
 * handler's constraint class.
 * It should allocate and fill the constraint data, and call SCIPcreateCons().
 * Take a look at the following example from the \ref cons_knapsack.h "knapsack constraint handler":
 *
 * @refsnippet{src/scip/cons_knapsack.c,SnippetConsCreationKnapsack}
 *
 * In this example, consdataCreate() is a local method that allocates memory for the given consdata
 * and fills the data with the given <code>vars</code> array. For allocating memory for the constraint data, you
 * can use SCIP memory allocation:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, consdata) );
 * \endcode
 *
 *
 * @section CONS_CALLBACKS Callback methods of Constraint handlers
 *
 * Besides the various functions which you will implement inside your constraint handler there exists a number
 * of <b> callback methods </b> associated with your constraint handler. Callback methods can be regarded as
 * tasks which your constraint handler is able to provide to the solver. They are grouped into two
 * categories:
 *
 * \ref CONS_FUNDAMENTALCALLBACKS "Fundamental Callback methods" are mandatory to implement
 * such that your code will work. For example, every constraint handler has to provide the
 * functionality to state whether all of its constraints are
 * fulfilled by a given variable assignment. Hence, the \ref CONSCHECK "CONSCHECK" callback is
 * one of the fundamental (or \a basic) callbacks of a constraint handler.
 *
 * Callbacks which are not necessarily implemented are grouped together as
 * \ref CONS_ADDITIONALCALLBACKS "additional callbacks". Such callbacks can be used to allocate and free memory
 * at different stages of the solving process. Although not mandatory, it might be useful to implement
 * some of these callbacks, e.g., to extend your constraint handler by a
 * \ref CONSSEPALP "separation" or \ref CONSPRESOL "presolving" functionality.
 *
 * All callbacks should be passed to SCIP during the SCIPinclude\<PLUGINTYPE\>\<PLUGINNAME\> method
 * (e.g., SCIPincludeConshdlrKnapsack() for the \ref cons_knapsack.h "knapsack constraint handler").
 * Since SCIP version 3.0, two ways of setting callbacks can be used, either via SCIPincludeConshdlr()
 * (all at once, as it always was), or via SCIPincludeConshdlrBasic() and setter functions for additional callbacks.
 * Since the basic inclusion methods are very unlikely to change and will thus
 * make your code more stable towards future versions of SCIP with more callbacks,
 * we recommend the latter choice, as explained in the \ref CONS_INTERFACE "interface" section.
 *
 * @section CONS_FUNDAMENTALCALLBACKS Fundamental Callback Methods
 *
 * By implementing the fundamental callbacks, you define the semantics of the constraint class the constraint handler
 * deals with.
 * If these methods are implemented, the resulting code is already correct and finds the optimal solution to the
 * given problem instance.
 * However, it might be very slow because the additional features, like cut separation and domain propagation, are
 * missing.
 * In the C++ wrapper class scip::ObjConshdlr, the fundamental callback methods are virtual abstract member functions.
 * You have to implement them in order to be able to construct an object of your constraint handler class.
 *
 * There are three fundamental callback methods that are all dealing with the feasibility of a given solution.
 * They are called at different places in the algorithm and have slightly different meaning.
 * However, it is usually reasonable to implement a single local method that is called by all of the three callback
 * methods with slightly modified parameters.
 * The fourth method provides dual information that is used for example in preprocessing.
 *
 * Additional documentation for the callback methods can be found in type_cons.h.
 *
 * @subsection CONSCHECK
 *
 * The CONSCHECK callback gets a primal solution candidate in a SCIP_SOL* data structure
 * and has to check this solution for global feasibility.
 * It has to return a result SCIP_FEASIBLE, if the solution satisfies all the constraints of the constraint handler,
 * and a result SCIP_INFEASIBLE if there is at least one constraint that is violated.
 *
 * If the solution is not NULL, SCIP should also be informed about the constraint violation with a call to
 * SCIPupdateSolConsViolation() and additionally SCIPupdateSolLPRowViolation() for every row of the constraint's current
 * representation in the LP relaxation, if any such rows exist.
 * As a convenience method, SCIPupdateSolLPConsViolation() can be used if the constraint
 * is represented completely by a set of LP rows, meaning that the current constraint violation is equal to the maximum
 * of the contraint violations of the corresponding LP rows.
 *
 * The callback is used by primal heuristics to check a constructed solution for feasibility.
 * That means, the constraint handler has to deal with arbitrary solutions that do not necessarily satisfy the bounds
 * and constraints of the local subproblem.
 *
 * The value of a variable \em var in the given solution \em sol can be accessed by calling
 * \code
 * SCIPgetSolVal(scip, sol, var)
 * \endcode
 *
 * For example, the \ref cons_knapsack.h "knapsack constraint handler" loops over its constraints and
 * calculates the scalar product \f$w^T x\f$ of weights \f$w\f$ with the solution vector \f$x\f$.
 * This scalar product is compared with the capacity of the knapsack constraint.
 * If it exceeds the capacity, the CONSCHECK method is immediately aborted with the result SCIP_INFEASIBLE.
 * If all knapsack constraints are satisfied, a result SCIP_FEASIBLE is returned.
 *
 * @subsection CONSENFOLP
 *
 * The CONSENFOLP method is called after the price-and-cut loop was finished and an LP solution is available.
 * Like the CHECK call, the ENFOLP method should return a result SCIP_FEASIBLE, if the solution satisfies all the
 * constraints.
 * However, the behavior should be different, if the solution violates some of the associated constraints.
 * The constraint handler may return a result SCIP_INFEASIBLE in this situation, but this is not the best what
 * one can do.
 * The ENFOLP method has the possibility of \em resolving the infeasibility by
 * - stating that the current subproblem is infeasible (result SCIP_CUTOFF),
 * - adding an additional constraint that resolves the infeasibility (result SCIP_CONSADDED),
 * - reducing the domain of a variable (result SCIP_REDUCEDDOM),
 * - adding a cutting plane (result SCIP_SEPARATED),
 * - tightening the LP primal feasibility tolerance and requesting to solve the LP again (result SCIP_SOLVELP),
 * - performing a branching (result SCIP_BRANCHED).
 *
 * However, the solution is not given as a SCIP_SOL* data structure.
 *
 * The value of a variable <code>var</code> in the LP solution can be accessed by calling
 * \code
 * SCIPgetVarSol(scip, var)
 * \endcode
 * or by
 * \code
 * SCIPgetSolVal(scip, NULL, var)
 * \endcode
 * By using the latter method, you can have a single local method to check a solution for feasibility by passing
 * the given <code>sol</code> to the CONSCHECK call and by passing a NULL pointer as <code>sol</code> to
 * the CONSENFOLP and CONSENFOPS calls.
 *
 *
 * @subsection CONSENFOPS
 *
 * The CONSENFOPS callback is similar to the CONSENFOLP callback, but deals with \em pseudo \em solutions instead
 * of LP solutions.
 *
 * If the LP was not solved at the current subproblem (either because the user did not want to solve it, or because
 * numerical difficulties in the LP solving process were detected) no LP solution is available.
 * In this situation, the pseudo solution is used instead.
 * In this solution, the variables are set to the local bound which is best with respect to the objective function.
 * You can think of the pseudo solution as solution to the LP relaxation with all constraints except the bounds
 * being removed.
 *
 * Like the ENFOLP callback, the ENFOPS callback has to check whether the pseudo solution satisfies all the constraints
 * of the constraint handler.
 * The pseudo solution can be accessed by the same methods as the LP solution (SCIP knows, if the LP was solved at the
 * current subproblem, and returns either the LP solution or the pseudo solution).
 *
 * Unlike the ENFOLP callback, the ENFOPS callback must not add cuts and cannot return the result SCIP_SEPARATED.
 * It is, however, possible to force the solving of the LP by returning the result SCIP_SOLVELP.
 * For example, the infeasibility of a linear constraint that contains continuous variables cannot be resolved,
 * if all integer variables in the constraint are already fixed.
 * In this case, the LP has to be solved in order to get a solution that satisfies the linear constraint.
 *
 * @subsection CONSENFORELAX
 *
 * The CONSENFORELAX callback is similar to the CONSENFOLP and CONSENFOPS callbacks, but deals with relaxation solutions.
 *
 * If the best bound computed by a relaxator that includes the whole LP is strictly better than the bound of the LP itself,
 * the corresponding relaxation solution will get enforced. Therefore the CONSENFORELAX callback will only be called for
 * solutions that satisfy all active LP-constraints.
 *
 * Like the ENFOLP and ENFOPS callbacks, the ENFORELAX callback has to check whether the solution given in sol satisfies
 * all the constraints of the constraint handler. Since the callback is only called for relaxators including the whole LP,
 * cuts may be added with a result of SCIP_SEPARATED, like in the ENFOLP callback. It is also possible to return
 * SCIP_SOLVELP if the relaxation solution is invalid for some reason and the LP should be solved instead.
 *
 * Note that the CONSENFORELAX callback is only relevant if relaxators are used. Since the basic distribution of the
 * SCIP Optimization Suite does not contain any relaxators, this callback can be ignored unless any relaxators are added
 * via user-plugins.
 *
 * @subsection CONSLOCK
 *
 * The CONSLOCK callback provides dual information for a single constraint.
 * It has to tell SCIP, which variables are existing in the given constraint, and in which way modifications of these
 * variables may affect the feasibility of the constraint.
 *
 * For each variable that is affected by the constraint, the callback should call SCIPaddVarLocks():
 *  - If the constraint may become violated by decreasing the value of a variable, it should call
 *    SCIPaddVarLocks(scip, var, nlockspos, nlocksneg), saying that rounding down is potentially rendering the
 *    (positive) constraint infeasible and rounding up is potentially rendering the negation of the constraint
 *    infeasible.
 *  - If the constraint may become violated by increasing the value of a variable, it should call
 *    SCIPaddVarLocks(scip, var, nlocksneg, nlockspos), saying that rounding up is potentially rendering the
 *    constraint's negation infeasible and rounding down is potentially rendering the constraint itself
 *    infeasible.
 *  - If the constraint may become violated by changing the variable in any direction, it should call
 *    SCIPaddVarLocks(scip, var, nlockspos + nlocksneg, nlockspos + nlocksneg).
 *
 *  <b>Note:</b> You do not have to worry about nlockspos and nlocksneg. These integer values are given as
 *  parameter of the CONSLOCK callback (see type_cons.h). Just use these variables in the above described
 *  fashion <b>without</b> adding or subtracting anything to them. In case of the knapsack constraints this
 *  method looks like this.
 *
 *  @refsnippet{src/scip/cons_knapsack.c,SnippetConsLockKnapsack}
 *
 *  To give same more intuition, consider the linear constraint \f$3x -5y +2z \leq 7\f$ as an example.
 *  The CONSLOCK callback method of the linear constraint handler should call
 *  SCIPaddVarLocks(scip, x, nlocksneg, nlockspos), SCIPaddVarLocks(scip, y, nlockspos, nlocksneg),
 *  and SCIPaddVarLocks(scip, z, nlocksneg, nlockspos) to tell SCIP,  that rounding up of \f$x\f$
 *  and \f$z\f$ and rounding down of \f$y\f$ can destroy the feasibility of the constraint, while rounding
 *  down of \f$x\f$ and \f$z\f$ and rounding up of \f$y\f$ can destroy the feasibility of the
 *  constraint's negation \f$3x -5y +2z > 7\f$.
 *  \n
 *  A linear constraint \f$2 \leq 3x -5y +2z \leq 7\f$ should call
 *  SCIPaddVarLocks(scip, ..., nlockspos + nlocksneg, nlockspos + nlocksneg) on all variables,
 *  since rounding in both directions of each variable can destroy both the feasibility of the
 *  constraint and it's negation \f$3x -5y +2z < 2\f$  or  \f$3x -5y +2z > 7\f$.
 *
 *
 * @section CONS_ADDITIONALCALLBACKS Additional Callback Methods
 *
 * The additional callback methods do not need to be implemented in every case, but provide useful functionality
 * for many applications. They can be added to your constraint handler via setter functions, see
 * \ref CONS_INTERFACE "here".
 *
 * @subsection CONSFREE
 *
 * If you are using constraint handler data, you have to implement this method in order to free the
 * constraint handler data. This can be done by the following procedure (which is taken from the
 * \ref cons_knapsack.h "knapsack constraint handler"):
 *
 * @refsnippet{src/scip/cons_knapsack.c,SnippetConsFreeKnapsack}
 *
 * If you have allocated memory for fields in your constraint handler data, remember to free this memory
 * before freeing the constraint handler data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 * @subsection CONSHDLRCOPY
 *
 * The CONSHDLRCOPY callback is executed when the SCIP instance is copied, e.g. to solve a sub-SCIP. By defining this
 * callback as <code>NULL</code> the user disables the inclusion of the specified constraint handler into all copied SCIP
 * instances. This may deteriorate the performance of primal heuristics solving sub-SCIPs, since these constitute only
 * relaxations of the original problem if constraint handlers are missing.
 *
 * A usual implementation just
 * calls the interface method which includes the constraint handler to the model. For example, this callback is
 * implemented for the \ref cons_knapsack.c "knapsack constraint handler" as follows:
 *
 * @refsnippet{src/scip/cons_knapsack.c,SnippetConsCopyKnapsack}
 *
 * <b>Note:</b> If you implement this callback, take care when setting the valid pointer.
 *
 * A problem copy is called valid if it is valid in both the primal and the dual sense, i.e., if
 *
 *  -   it is a relaxation of the source problem
 *  -   it does not enlarge the feasible region.
 *
 * A constraint handler may choose to not copy a constraint and still declare the resulting copy as valid. It must ensure
 * the feasibility of any solution to the problem copy in the original (source) space.
 *
 * <b>Note:</b> If you implement this callback and the constraint handler needs constraints (see CONSHDLR_NEEDSCONS),
 * then you also need to implement the callback \ref CONSCOPY.
 *
 * @subsection CONSINIT
 *
 * The CONSINIT callback is executed after the problem is transformed.
 * The constraint handler may, e.g., use this call to replace the original variables in its constraints by transformed
 * variables, or to initialize its statistical constraint handler data.
 *
 * @subsection CONSEXIT
 *
 * The CONSEXIT callback is executed before the transformed problem is freed.
 * In this method, the constraint handler should free all resources that were allocated for the solving process.
 *
 * @subsection CONSINITPRE
 *
 * The CONSINITPRE callback is executed before the preprocessing is started, even if presolving is turned off.
 * The constraint handler may use this call to initialize its presolving data, or to modify its constraints
 * before the presolving process begins.
 * Necessary constraint modifications that have to be performed even if presolving is turned off should be done here
 * or in the presolving deinitialization call.
 *
 * @subsection CONSEXITPRE
 *
 * The CONSEXITPRE callback is executed after the preprocessing has been finished, even if presolving is turned off.
 * The constraint handler may use this call e.g. to clean up its presolving data, or to finally modify its constraints
 * before the branch-and-bound process begins.
 * Necessary constraint modifications that have to be performed even if presolving is turned off should be done here
 * or in the presolving initialization call.
 * Besides necessary modifications and clean up, no time consuming operations should be done.
 *
 * @subsection CONSINITSOL
 *
 * The CONSINITSOL callback is executed when the presolving is finished and the branch-and-bound process is about to
 * begin.
 * The constraint handler may use this call to initialize its branch-and-bound specific data.
 *
 * @subsection CONSEXITSOL
 *
 * The CONSEXITSOL callback is executed before the branch-and-bound process is freed.
 * The constraint handler should use this call to clean up its branch-and-bound data, in particular to release
 * all LP rows that it has created or captured.
 *
 * @subsection CONSDELETE
 *
 * The CONSDELETE callback is executed if a constraint should be freed.
 * You can think of it as the destructor of a single constraint.
 * In the callback, you have to free the given constraint data.
 * The CONSDELETE callback is therefore the counterpart of the SCIPcreateCons...() interface method and the CONSTRANS
 * method.
 *
 * @subsection CONSTRANS
 *
 * The CONSTRANS method is called for each constraint of the constraint handler, when the user starts the solving
 * process.
 * It has to copy the original constraint data of the constraint to the memory for the transformed problem.
 * You can think of it as a copy constructor for a single constraint.
 *
 * The original model is copied in order to protect it from transformations that are applied during the solving process,
 * in particular during preprocessing.
 * Preprocessing and solving always operates on the transformed problem.
 * If the solving process data are freed, the original data still exist and the user can, e.g., modify the problem and
 * restart the solving process.
 *
 * If you do not implement the CONSTRANS method, a transformed constraint is created with the same flags and the
 * same constraint data pointer.
 * That means, the transformed constraint points to the original constraint data.
 * This is okay, as long as the constraint data is not changed during the solving process.
 * If you want to implement preprocessing methods or other methods that modify the constraint data, you have to
 * implement the CONSTRANS method and create a copy of the constraint data.
 *
 * Here is an example, which is taken from the \ref cons_knapsack.h "knapsack constraint handler":
 *
 * @refsnippet{src/scip/cons_knapsack.c,SnippetConsTransKnapsack}
 *
 * @subsection CONSINITLP
 *
 * The CONSINITLP callback is executed before the first LP relaxation is solved.
 * It should add the LP relaxations of all "initial" constraints to the LP. The method should scan the constraints
 * array for constraints that are marked initial via calls to SCIPconsIsInitial() and put the LP relaxation
 * of all initial constraints to the LP with calls to SCIPaddCut().
 *
 * @subsection CONSSEPALP
 *
 * The CONSSEPALP callback is executed during the price-and-cut loop of the subproblem processing.
 * It should try to generate cutting planes for the constraints of the constraint handler in order to separate
 * the current LP solution.
 * The method is called in the LP solution loop, which means that a valid LP solution exists.
 *
 * Usually, a separation callback searches and produces cuts, that are added with a call to SCIPaddCut().
 * If the cut should be remembered in the global cut pool, it may also call SCIPaddPoolCut().
 * However, the callback may also produce domain reductions or add other constraints.
 *
 * The CONSSEPALP callback has the following options:
 *  - detecting that the node is infeasible in the variables' bounds and can be cut off (result SCIP_CUTOFF)
 *  - adding an additional constraint (result SCIP_CONSADDED)
 *  - reducing a variable's domain (result SCIP_REDUCEDDOM)
 *  - adding a cutting plane to the LP (result SCIP_SEPARATED)
 *  - stating that the separator searched, but did not find domain reductions, cutting planes, or cut constraints
 *    (result SCIP_DIDNOTFIND)
 *  - stating that the separator was skipped (result SCIP_DIDNOTRUN)
 *  - stating that the separator was skipped, but should be called again (result SCIP_DELAYED)
 *  - stating that a new separation round should be started without calling the remaining separator methods (result SCIP_NEWROUND)
 *
 * Please see also the @ref CONS_ADDITIONALPROPERTIES section to learn about the properties
 * CONSHDLR_SEPAFREQ, CONSHDLR_SEPAPRIORITY, and CONSHDLR_DELAYSEPA, which influence the behaviour of SCIP
 * calling CONSSEPALP.
 *
 * @subsection CONSSEPASOL
 *
 * The CONSSEPASOL callback is executed during separation loop on arbitrary primal solutions.
 * It should try to generate cutting planes for the constraints of the constraint handler in order to separate
 * the given primal solution.
 * The method is not called in the LP solution loop, which means that there is no valid LP solution.
 *
 * Usually, a separation callback searches and produces cuts, that are added with a call to SCIPaddCut().
 * If the cut should be remembered in the global cut pool, it may also call SCIPaddPoolCut().
 * However, the callback may also produce domain reductions or add other constraints.
 *
 * The CONSSEPASOL callback has the following options:
 *  - detecting that the node is infeasible in the variables' bounds and can be cut off (result SCIP_CUTOFF)
 *  - adding an additional constraint (result SCIP_CONSADDED)
 *  - reducing a variable's domain (result SCIP_REDUCEDDOM)
 *  - adding a cutting plane to the LP (result SCIP_SEPARATED)
 *  - stating that the separator searched, but did not find domain reductions, cutting planes, or cut constraints
 *    (result SCIP_DIDNOTFIND)
 *  - stating that the separator was skipped (result SCIP_DIDNOTRUN)
 *  - stating that the separator was skipped, but should be called again (result SCIP_DELAYED)
 *  - stating that a new separation round should be started without calling the remaining separator methods (result SCIP_NEWROUND)
 *
 * Please see also the @ref CONS_ADDITIONALPROPERTIES section to learn about the properties
 * CONSHDLR_SEPAFREQ, CONSHDLR_SEPAPRIORITY, and CONSHDLR_DELAYSEPA, which influence the behaviour of SCIP
 * calling CONSSEPASOL.
 *
 * @subsection CONSPROP
 *
 * The CONSPROP callback is called during the subproblem processing.
 * It should propagate the constraints, which means that it should infer reductions in the variables' local bounds
 * from the current local bounds.
 * This technique, which is the main workhorse of constraint programming, is called "node preprocessing" in the
 * Integer Programming community.
 *
 * The CONSPROP callback has the following options:
 *  - detecting that the node is infeasible in the variables' bounds and can be cut off (result SCIP_CUTOFF)
 *  - reducing a variable's domain (result SCIP_REDUCEDDOM)
 *  - stating that the propagator searched, but did not find domain reductions, cutting planes, or cut constraints
 *    (result SCIP_DIDNOTFIND)
 *  - stating that the propagator was skipped (result SCIP_DIDNOTRUN)
 *  - stating that the propagator was skipped, but should be called again (result SCIP_DELAYED)
 *
 * Please see also the @ref CONS_ADDITIONALPROPERTIES section to learn about the properties
 * CONSHDLR_PROPFREQ, CONSHDLR_DELAYPROP, and CONSHDLR_PROP_TIMING, which influence the behaviour of SCIP
 * calling CONSPROP.
 *
 * @subsection CONSRESPROP
 *
 * If the constraint handler should support \ref CONF "conflict analysis", it has to supply a CONSRESPROP method.
 * It also should call SCIPinferVarLbCons() or SCIPinferVarUbCons() in domain propagation instead of SCIPchgVarLb() or
 * SCIPchgVarUb() in order to deduce bound changes on variables.
 * In the SCIPinferVarLbCons() and SCIPinferVarUbCons() calls, the handler provides the constraint that deduced the
 * variable's bound change, and an integer value <code>inferinfo</code> that can be arbitrarily chosen.
 *
 * The propagation conflict resolving method CONSRESPROP must then be implemented to provide the "reasons" for the bound
 * changes, i.e., the bounds of variables at the time of the propagation, which forced the constraint to set the
 * conflict variable's bound to its current value. It can use the <code>inferinfo</code> tag to identify its own propagation rule
 * and thus identify the "reason" bounds. The bounds that form the reason of the assignment must then be provided by
 * calls to SCIPaddConflictLb() and SCIPaddConflictUb() in the propagation conflict resolving method.
 *
 * <b>Note:</b> The fact that <code>inferinfo</code> is an integer, as opposed to an arbitrary data object, is a compromise between space and speed. Sometimes a propagator would
 * need more information to efficiently infer the original propagation steps that lead to the conflict. This would,
 * however, require too much space. In the extreme, the original propagation steps have to be repeated.
 *
 * For example, the \ref cons_logicor.h "logicor constraint" \f$c = x \vee y \vee z\f$ fixes variable \f$z\f$ to TRUE (i.e., changes the lower
 * bound of \f$z\f$ to 1.0), if both, \f$x\f$ and \f$y\f$, are assigned to FALSE (i.e., if the upper bounds of these
 * variables are 0.0). It uses <code>SCIPinferVarLbCons(scip, z, 1.0, c, 0)</code> to apply this assignment (an
 * inference information tag is not needed by the constraint handler and is set to 0).  In the conflict analysis, the
 * constraint handler may be asked to resolve the lower bound change on \f$z\f$ with constraint \f$c\f$, that was
 * applied at a time given by a bound change index "bdchgidx".  With a call to <code>SCIPvarGetLbAtIndex(z,
 * bdchgidx)</code>, the handler can find out, that the lower bound of variable \f$z\f$ was set to 1.0 at the given
 * point of time, and should call <code>SCIPaddConflictUb(scip, x, bdchgidx)</code> and <code>SCIPaddConflictUb(scip, y,
 * bdchgidx)</code> to tell SCIP, that the upper bounds of \f$x\f$ and \f$y\f$ at this point of time were the reason for
 * the deduction of the lower bound of \f$z\f$.
 *
 * If conflict analysis should not be supported, the method has to set the result code to SCIP_DIDNOTFIND.  Although
 * this is a viable approach to circumvent the implementation of the usually rather complex conflict resolving method, it
 * will make the conflict analysis less effective. We suggest to first omit the conflict resolving method and check how
 * effective the \ref CONSPROP "propagation method" is. If it produces a lot of propagations for your application, you definitely should
 * consider implementing the conflict resolving method.
 *
 * @subsection CONSPRESOL
 *
 * The CONSPRESOL callback is called during preprocessing.
 * It should try to tighten the domains of the variables, tighten the coefficients of the constraints of the constraint
 * handler, delete redundant constraints, aggregate and fix variables if possible, and upgrade constraints to more
 * specific types.
 *
 * If the CONSPRESOL callback applies changes to the constraint data, you also have to implement the \ref CONSTRANS callback
 * in order to copy the constraint data to the transformed problem space and protect the original problem from the
 * preprocessing changes.
 *
 * To inform SCIP that the presolving method found a reduction the result pointer has to be set in a proper way.
 * The following options are possible:
 *
 *  - SCIP_UNBOUNDED  : at least one variable is not bounded by any constraint in objective direction
 *  - SCIP_CUTOFF     : at least one constraint is infeasible in the variable's bounds
 *  - SCIP_SUCCESS    : the presolver found a reduction
 *  - SCIP_DIDNOTFIND : the presolver searched, but did not find a presolving change
 *  - SCIP_DIDNOTRUN  : the presolver was skipped
 *  - SCIP_DELAYED    : the presolver was skipped, but should be called again
 *
 * Please see also the @ref CONS_ADDITIONALPROPERTIES section to learn about the properties
 * CONSHDLR_PRESOLTIMING and CONSHDLR_MAXPREROUNDS, which influence the behaviour of SCIP
 * calling CONSPRESOL.
 *
 * @subsection CONSACTIVE
 *
 * The CONSACTIVE callback method is called each time a constraint of the constraint handler is activated.
 * For example, if a constraint is added locally to a subproblem, the CONSACTIVE callback is called whenever the
 * search enters the subtree where the constraint exists.
 *
 * @subsection CONSDEACTIVE
 *
 * The CONSDEACTIVE callback method is called each time a constraint of the constraint handler is deactivated.
 * For example, if a constraint is added locally to a subproblem, the CONSDEACTIVE callback is called whenever the
 * search leaves the subtree where the constraint exists.
 *
 * @subsection CONSENABLE
 *
 * The CONSENABLE callback method is called each time a constraint of the constraint handler is enabled.
 * Constraints might be active without being enabled. In this case, only the feasibility checks are executed,
 * but domain propagation and separation is skipped.
 *
 * @subsection CONSDISABLE
 *
 * The CONSDISABLE callback method is called each time a constraint of the constraint handler is disabled.
 *
 * @subsection CONSPRINT
 *
 * The CONSPRINT callback method is called, when the user asks SCIP to display the problem to the screen
 * or save the problem into a file. This is, however, only the case if the user requested the CIP format.
 * For more details about reading and writing with SCIP we refer to the \ref READER "file readers". In this
 * callback method the constraint handler should display the data of the constraint in an appropriate form.
 * The output format that is defined by the CONSPRINT callbacks is called CIP format.
 * In later versions of SCIP, the constraint handlers should also be able to parse (i.e., read) constraints
 * which are given in CIP format.
 *
 * @subsection CONSCOPY
 *
 * The CONSCOPY callback method is used whenever constraints should be copied from one SCIP instance into another SCIP
 * instance. This method comes with the necessary parameters to do so, most importantly with a mapping of the variables of the
 * source SCIP instance to the corresponding variables of the target SCIP instance, and a mapping for the constraints
 * in the same way. For a complete list of all arguments of this callback method see type_cons.h.
 *
 * To get the corresponding target variable of a given source variable, you can use the variable map directly:
 *
 * \code
 * targetvar = (SCIP_VAR*) SCIPhashmapGetImage(varmap, sourcevar);
 * \endcode
 *
 * We recommend, however, to use the method SCIPgetVarCopy() which gets besides others the variable map and the constraint map as input
 * and returns the requested target variable. The advantage of using SCIPgetVarCopy() is that in the case
 * the required variable does not yet exist, it is created and added to the copy automatically:
 *
 * \code
 * SCIP_CALL( SCIPgetVarCopy(sourcescip, scip, sourcevar, &targetvar, varmap, consmap, global) );
 * \endcode
 *
 * Finally, the result pointer <code>valid</code> has to be set to TRUE if (and only if!) the copy process was successful.
 *
 * <b>Note:</b> Be careful when setting the valid pointer.
 * A problem copy is called valid if it is valid in both the primal and the dual sense, i.e., if
 *
 *  -   it is a relaxation of the source problem
 *  -   it does not enlarge the feasible region.
 *
 * A constraint handler may choose to not copy a constraint and still declare the resulting copy as valid. Therefore, it must ensure
 * the feasibility of any solution to the problem copy in the original (source) space.
 *
 * For an example implementation we refer to cons_linear.h. Additional documentation and the complete list of all
 * parameters can be found in the file in type_cons.h.
 *
 * @subsection CONSPARSE
 *
 * This method is the counter part to CONSPRINT. The ideal idea is that a constraint handler is able to parse the output
 * which it generated via the CONSPRINT method and creates the corresponding constraint. If the parsing was successfully
 * the result pointer success should be set to TRUE. An example implementation can be found in the \ref cons_linear.h
 * "linear constraint handler".
 *
 * @subsection CONSDELVARS
 *
 * This method should iterate over the given constraints and delete all variables that were marked for deletion by SCIPdelVar().
 * Variable deletion is especially interesting for branch-cut-and-price applications. If your constraint handler allows
 * the addition of variables during the solving process (see "modifiable" attribute of constraints), then you might also want to
 * implement this callback. This would allow you to not only create variables during solving, but also remove them dynamically
 * from the problem to reduce memory consumption in case they are no longer necessary.
 * During presolving, SCIP may also find that some variables are not needed anymore and then try
 * to delete them. Thus, if you do not implement this callback, the constraint handler should capture its variables via
 * SCIPcaptureVar() to prevent SCIP from erroneously deleting them.
 *
 * Additional documentation and the complete list of all parameters can be found in the file type_cons.h.
 *
 * @subsection CONSGETVARS
 *
 * The CONSGETVARS callback of a constraint handler can be implemented to give access to the constraint variables
 * as array, independently from the internal data structure of the constraint. The buffer array
 * is already passed, together with its length. Consider implementing @ref CONSGETNVARS, too, to have
 * information about the number of variables in this constraint.
 *
 * @subsection CONSGETNVARS
 *
 * This callback can be implemented to return the number of variables involved into a particular constraint.
 * In order to have access to the variable pointers, consider implementing @ref CONSGETVARS.
 *
 * @refsnippet{src/scip/cons_linear.c,Callback for the number of variables}
 *
 * @subsection CONSGETDIVEBDCHGS
 *
 *  This callback is used inside the various diving heuristics of SCIP and does not affect the normal branching
 *  of the actual search.
 *  The constraint handler can provide this callback to render a current working solution (even more) infeasible by
 *  suggesting one or several variable bound changes.
 *
 * @section CONS_FURTHERINFO Further documentation
 *
 * Further documentation can be found in @ref type_cons.h for callback descriptions and a complete
 * list of all callback parameters, or in @ref scip.h
 * for globally available functions.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page PRICER How to add variable pricers
 *
 * A pricer performs the dynamic generation of new variables in a column generation algorithm.
 * It is an algorithmic representation of a (usually exponential) number of variables.
 * The \ref PRICERREDCOST and \ref PRICERFARKAS methods are called after each LP solve to generate additional
 * variables which may improve the objective value or decrease the LP infeasibility, respectively.
 * \n
 * A complete list of all pricers contained in this release can be found \ref PRICERS "here".
 *
 * If the pricer finds one or more variables with negative reduced costs or negative Farkas value, it should
 * call SCIPcreateVar() and SCIPaddPricedVar() to create and add the variable to the problem. Additionally,
 * the pricer has to add the variable to all constraints in which it appears. Therefore, a pricer needs to
 * know the constraints of the model and their meaning. Note that all constraints for which additional variables
 * are generated by a pricer have to be flagged as "modifiable" in the SCIPcreateCons() call.
 *
 * We now explain how users can add their own pricers.
 * For example, look into the variable pricer for the binpacking problem (examples/Binpacking/src/pricer_binpacking.c) of the
 * Binpacking example project.
 * The example is written in C. C++ users can easily adapt the code by using the scip::scip::ObjPricer wrapper base class and
 * implement the scip_...() virtual methods instead of the SCIP_DECL_PRICER... callback methods.
 *
 * Additional documentation for the callback methods of a pricer can be found in the file
 * type_pricer.h.
 *
 * Notice that if your pricer cannot cope with variable bounds other than 0 and infinity, you have to mark
 * all constraints containing priced variables as modifiable, and you may have to disable reduced cost
 * strengthening by setting propagating/rootredcost/freq to -1.
 *
 * Here is what you have to do to implement a pricer:
 * -# Copy the template files src/scip/pricer_xyz.c and src/scip/pricer_xyz.h into files "pricer_mypricer.c"
 *    and "pricer_mypricer.h".
 *    \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPincludePricerMypricer() in order to include the pricer into your SCIP instance,
 *    e.g., in the main file of your project (see, e.g., src/cmain.c in the Binpacking example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "mypricer".
 * -# Adjust the properties of the pricer (see \ref PRICER_PROPERTIES).
 * -# Define the pricer data (see \ref PRICER_DATA). This is optional.
 * -# Implement the interface methods (see \ref PRICER_INTERFACE).
 * -# Implement the fundamental callback methods (see \ref PRICER_FUNDAMENTALCALLBACKS).
 * -# Implement the additional callback methods (see \ref PRICER_ADDITIONALCALLBACKS).  This is optional.
 *
 *
 * @section PRICER_PROPERTIES Properties of a Pricer
 *
 * At the top of the new file "pricer_mypricer.c" you can find the pricer properties.
 * These are given as compiler defines.
 * In the C++ wrapper class, you have to provide the pricer properties by calling the constructor
 * of the abstract base class scip::ObjPricer from within your constructor.
 * The properties you have to set have the following meaning:
 *
 * \par PRICER_NAME: the name of the pricer.
 * This name is used in the interactive shell to address the pricer.
 * Additionally, if you are searching for a pricer with SCIPfindPricer(), this name is looked up.
 * Names have to be unique: no two pricers may have the same name.
 *
 * \par PRICER_DESC: the description of the pricer.
 * This string is printed as a description of the pricer in the interactive shell.
 *
 * \par PRICER_PRIORITY: the priority of the pricer.
 * In each pricing round during the price-and-cut loop of the subproblem processing, the included pricers are
 * called in a predefined order, which is given by the priorities of the pricers.
 * The higher the priority, the earlier the pricer is called.
 * Usually, you will have only one pricer in your application and the priority is therefore irrelevant.
 *
 * \par PRICER_DELAY: the default for whether the pricer should be delayed, if other variables with negative reduced
 * costs have already been found in the current pricing round.
 * Variables may be declared to be "removable" in the SCIPcreateVar() call. This means that SCIP may remove the variable
 * from the LP if it was inactive (i.e., sitting at zero) for a number of LP solves. Nevertheless, after the removal of the
 * column from the LP, the variable still exists, and SCIP can calculate reduced costs and add it to the LP again if
 * necessary.
 * \n
 * If the PRICER_DELAY flag is set to TRUE (which is the common setting), all those existing variables with negative reduced costs
 * are added to the LP, and the LP is resolved before the pricer is called. Thus, the pricer can assume that all existing variables
 * have non-negative reduced costs if the \ref PRICERREDCOST method is called or non-positive Farkas value if the \ref PRICERFARKAS
 * method is called.
 * \n
 * In some applications, this inner pricing loop on the already existing variables can significantly slow down the solving process,
 * since it may lead to the addition of only very few variables in each pricing round. If this is an issue in your application,
 * you should consider setting the PRICER_DELAY flag to FALSE. You must, however, be aware of the fact that there may be already
 * existing variables with negative reduced costs. For example, this may lead to the issue that your pricer generates the same
 * variable twice. In some models, this is not critical because an optimal solution would choose only one of the two identical
 * variables anyway, but for other models this can lead to wrong results because the duplication of a variable essentially doubles
 * the upper bound of the variable.
 *
 *
 * @section PRICER_DATA Pricer Data
 *
 * Below the header "Data structures" you can find a struct which is called "struct SCIP_PricerData".
 * In this data structure, you can store the data of your pricer. For example, it may be convenient to store pointers to the
 * constraints of the problem instance here, because the pricer has to add variables to those constraints.
 * If you are using C++, you can add pricer data, as usual, as object variables to your class.
 * \n
 * Defining pricer data is optional. You can leave the struct empty.
 *
 *
 * @section PRICER_INTERFACE Interface Methods
 *
 * At the bottom of "pricer_mypricer.c" you can find the interface method SCIPincludePricerMypricer(), which also appears in "pricer_mypricer.h".
 * It is called by the user, if (s)he wants to include the pricer, i.e., if (s)he wants to solve a model for which variables should
 * be generated by this pricer.
 *
 * This method only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the pricer. For this, you can either call SCIPincludePricer(),
 * or SCIPincludePricerBasic() since SCIP version 3.0. In the latter variant, \ref PRICER_ADDITIONALCALLBACKS "additional callbacks"
 * must be added via setter functions as, e.g., SCIPsetPricerCopy(). We recommend this latter variant because
 * it is more stable towards future SCIP versions which might have more callbacks, whereas source code using the first
 * variant must be manually adjusted with every SCIP release containing new callbacks for pricers in order to compile.
 *
 *
 * In addition, the pricer has to be activated before the solution process starts, like it is done
 * in the pricer of the Coloring application (applications/Coloring/src/reader_col.c) by calling
 * \code
 * SCIP_CALL( SCIPactivatePricer(scip, SCIPfindPricer(scip, "coloring")) );
 * \endcode
 *
 * If you are using pricer data, you have to allocate the memory for the data at this point.
 * You can do this by calling:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &pricerdata) );
 * \endcode
 * You also have to initialize the fields in struct SCIP_PricerData afterwards.
 *
 * You may also add user parameters for your pricer, see the method SCIPincludePricerColoring() in the pricer of the Coloring application
 * for an example of how to add user parameters.
 *
 *
 * @section PRICER_FUNDAMENTALCALLBACKS Fundamental Callback Methods of a Pricer
 *
 * The fundamental callback methods have to be implemented in order to obtain an operational algorithm.
 * They are passed together with the pricer itself to SCIP using SCIPincludePricer() or SCIPincludePricerBasic(),
 * see @ref PRICER_INTERFACE.
 *
 * In the case of a pricer, there are two fundamental callback methods, namely the @ref PRICERREDCOST and the
 * @ref PRICERFARKAS callbacks, which both search for new variables and add them to the problem.
 * These methods have to be implemented for every pricer; the other callback methods are optional.
 * In the C++ wrapper class scip::ObjPricer, the scip_redcost() method (which corresponds to the PRICERREDCOST callback)
 * is a virtual abstract member function. You have to implement it in order to be able to construct an object of your
 * pricer class.
 *
 * Additional documentation for the callback methods can be found in type_pricer.h.
 *
 * @subsection PRICERREDCOST
 *
 * The PRICERREDCOST callback is called inside the price-and-cut loop of the subproblem solving process if the current LP relaxation
 * is feasible.
 * It should search for additional variables that can contribute to improve the current LP's solution value.
 * In standard branch-and-price, these are variables with negative dual feasibility, that is negative
 * reduced costs for non-negative variables, positive reduced costs for non-positive variables,
 * and non-zero reduced costs for variables that can be negative and positive.
 *
 * Whenever the pricer finds a variable with negative dual feasibility, it should call SCIPcreateVar()
 * and SCIPaddPricedVar() to add the variable to the problem. Furthermore, it should call the appropriate
 * methods of the constraint handlers to add the necessary variable entries to the constraints, see pub_cons.h.
 *
 * In the usual case that the pricer either adds a new variable or ensures that there are no further variables with negative dual feasibility,
 * the result pointer should be set to SCIP_SUCCESS. Only if the pricer aborts pricing without creating a new variable, but
 * there might exist additional variables with negative dual feasibility, the result pointer should be set to SCIP_DIDNOTRUN.
 * In this case, which sometimes is referred to as "early branching", the LP solution will not be used as a lower bound.
 * The pricer can, however, store a valid lower bound in the <code>lowerbound</code> pointer.
 *
 * Pricers usually need the dual LP solution as input for the pricing algorithm.
 * Since SCIP does not know the semantics of the individual constraints in the problem, the dual solution
 * has to be provided by the constraint handlers.
 * For example, the \ref cons_setppc.h "setppc constraint handler", which deals with set partitioning, packing, and covering constraints, provides
 * the method SCIPgetDualsolSetppc() to access the dual solution value for a single constraint.
 * Similarly, the dual solution of a linear constraint can be queried with the method SCIPgetDualsolLinear() of cons_linear.h.
 * The reduced costs of the existing variables can be accessed with the method SCIPgetVarRedcost().
 *
 * @subsection PRICERFARKAS
 *
 * If the current LP relaxation is infeasible, it is the task of the pricer to generate additional variables that can
 * potentially render the LP feasible again. In standard branch-and-price, these are variables with positive Farkas values,
 * and the PRICERFARKAS method should identify those variables.
 *
 * If the LP was proven to be infeasible, we have an infeasibility proof by the dual Farkas multipliers \f$y\f$.
 * With the values of \f$y\f$, an implicit inequality \f$y^T A x \ge y^T b\f$ is associated, with \f$b\f$ given
 * by the sides of the LP rows and the sign of \f$y\f$:
 *  - if \f$y_i\f$ is positive, \f$b_i\f$ is the left hand side of the row,
 *  - if \f$y_i\f$ is negative, \f$b_i\f$ is the right hand side of the row.
 *
 * \f$y\f$ is chosen in a way, such that the valid inequality  \f$y^T A x \ge y^T b\f$  is violated by all \f$x\f$,
 * especially by the (for this inequality least infeasible solution) \f$x'\f$ defined by
 *  - \f$x'_i := ub_i\f$, if \f$y^T A_i \ge 0\f$
 *  - \f$x'_i := lb_i\f$, if \f$y^T A_i < 0\f$.
 * Pricing in this case means to add variables \f$i\f$ with positive Farkas value, i.e., \f$y^T A_i x'_i > 0\f$.
 *
 * To apply Farkas pricing, the pricer needs to know the Farkas values of the constraints. Like the dual solution values for
 * feasible LP solutions, the dual Farkas values for infeasible solutions can be obtained by constraint handler interface
 * methods such as the SCIPgetDualfarkasLinear() method of the linear constraint handler.
 * The Farkas values for the bounds of the variables can be accessed with SCIPgetVarFarkasCoef().
 *
 * It is useful to note that Farkas pricing is the same as the regular pricing with a zero objective function.
 * Therefore, a typical implementation of a pricer would consist of a generic pricing algorithm that gets a dual solution and an
 * objective function vector as input and generates variables by calling SCIPcreateVar() and SCIPaddPricedVar().
 * The PRICERREDCOST callback would call this function with the regular objective function and the regular dual solution vector,
 * while the PRICERFARKAS callback would call this function with a zero objective function and the Farkas vector.
 * From a practical point of view, it is usually the simplest approach to provide just one Boolean flag to the generic pricing
 * algorithm in order to identify whether it is reduced cost or Farkas pricing. Then, the algorithm would just call the appropriate
 * methods to access the dual solution or objective function, depending on the Boolean flag.
 *
 * @section PRICER_ADDITIONALCALLBACKS Additional Callback Methods of a Pricer
 *
 * The additional callback methods do not need to be implemented in every case.
 * However, some of them have to be implemented for most applications. They can either be passed directly with
 * SCIPincludePricer() to SCIP or via specific <b>setter functions</b> after a call of SCIPincludePricerBasic(),
 * see also @ref PRICER_INTERFACE.
 *
 * @subsection PRICERFREE
 *
 * If you are using pricer data, you have to implement this method in order to free the pricer data.
 * This can be done by the following procedure:
 *
 * @refsnippet{applications/STP/src/pricer_stp.c,SnippetPricerFreeSTP}
 *
 * If you have allocated memory for fields in your pricer data, remember to free this memory
 * before freeing the pricer data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 * @subsection PRICERCOPY
 *
 * The PRICERCOPY callback is executed when the SCIP instance is copied, e.g. to solve a sub-SCIP. By defining this
 * callback as <code>NULL</code> the user disables the inclusion of the pricer into all copied SCIP
 * instances. This means that primal heuristics will work on a sub-SCIP that contains only a part of the variables
 * and no variables are priced in during the solving process of the sub-SCIP. Therefore, primal solutions found in the
 * copied problem are typically still valid for the original problem and used for its solving process,
 * but dual reductions cannot be transferred to the original problem.
 *
 * <b>Note:</b> If you implement this callback, be careful when setting the valid pointer. The valid pointer should be
 * set to TRUE if (and only if!) you can make sure that all necessary data of the pricer are copied
 * correctly. If the complete problem is validly copied, i.e. if the copy methods of all problem defining plugins
 * (constraint handlers and pricers) return <code>*valid = TRUE</code>, then dual reductions found for the copied problem can be
 * transferred to the original SCIP instance. Thus, if the valid pointer is wrongly set to TRUE, it might happen that
 * optimal solutions are cut off.
 *
 * @subsection PRICERINIT
 *
 * The PRICERINIT callback is executed after the problem is transformed.
 * The pricer may, e.g., use this call to replace the original constraints stored in its pricer data by transformed
 * constraints, or to initialize other elements of its pricer data.
 *
 * @subsection PRICEREXIT
 *
 * The PRICEREXIT callback is executed before the transformed problem is freed.
 * In this method, the pricer should free all resources that have been allocated for the solving process in PRICERINIT.
 *
 * @subsection PRICERINITSOL
 *
 * The PRICERINITSOL callback is executed when the presolving is finished and the branch-and-bound process is about to begin.
 * The pricer may use this call to initialize its branch-and-bound specific data.
 *
 * @subsection PRICEREXITSOL
 *
 * The PRICEREXITSOL callback is executed before the branch-and-bound process is freed.
 * The pricer should use this call to clean up its branch-and-bound data, which was allocated in PRICERINITSOL.
 *
 * @section PRICER_REMARKS Further remarks
 *
 * If you use your own branching rule (e.g., to branch on constraints), make sure that it is able to branch on \a "pseudo solutions".
 * Otherwise, SCIP will use its default branching rules, if necessary (which all branch on variables). This
 * could disturb the pricing problem or branching might not even be possible, e.g., if all variables created thus far have already been fixed.
 *
 * Note that if the original problem is a maximization problem, SCIP will transform the problem into a minimization
 * problem by multiplying the objective function by -1. The pricer has to take care of this by multiplying
 * the original objective function value of all variables created during the solving process by -1.
 *
 * In some cases, bounds on variables are implicitly enforced by constraints of the problem and the objective function.
 * Therefore, these bounds do not need to be added to the LP explicitly, which has the advantage that the pricing routine does not need to
 * care about the corresponding dual values.
 * We call these bounds lazy bounds, they may be set by SCIPchgVarLbLazy() and SCIPchgVarUbLazy() for upper or lower bounds, respectively.
 * If the lazy bound is tighter than the local bound, the corresponding bound is not put into the LP.
 * In diving mode, lazy bounds are explicitly put into the LP, because changing the objective (which is only possible in diving)
 * might reverse the implicitly given bounds. When diving is finished, the bounds are again removed from the LP.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page PRESOL How to add presolvers
 *
 * Presolvers are used to reduce the size of the model by removing irrelevant information like redundant constraints,
 * to strengthen the LP relaxation by exploiting integrality information, and to extract useful information in the
 * presolving step.
 * Constraint based presolving is done in the CONSPRESOL callback methods of the constraint handlers, see \ref CONSPRESOL.
 * Some propagation steps can already be applied in presolving via the PROPRESOL callback methods of propagators, see \ref PROPPRESOL.
 * The presolver plugins complement these by additional, usually optimality based, presolving reductions.
 * \n
 * A complete list of all presolvers contained in this release can be found \ref PRESOLVERS "here".
 *
 * We now explain how users can add their own presolvers.
 * Take the trivial presolver (src/scip/presol_trivial.c) as an example.
 * As all other default plugins, it is written in C. C++ users can easily adapt the code by using the scip::ObjPresol wrapper
 * base class and implement the scip_...() virtual methods instead of the SCIP_DECL_PRESOL... callback methods.
 *
 * Additional documentation for the callback methods of a presolver, in particular for their input parameters,
 * can be found in the file type_presol.h.
 *
 * Here is what you have to do to implement a presolver:
 * -# Copy the template files src/scip/presol_xyz.c and src/scip/presol_xyz.h into files named "presol_mypresolver.c"
 *    and "presol_mypresolver.h".
 *    \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPincludePresolMypresolver() in order to include the presolver into your SCIP instance,
 *    e.g., in the main file of your project (see, e.g., src/cmain.c in the Binpacking example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "mypresolver".
 * -# Adjust the properties of the presolver (see \ref PRESOL_PROPERTIES).
 * -# Define the presolver data (see \ref PRESOL_DATA). This is optional.
 * -# Implement the interface methods (see \ref PRESOL_INTERFACE).
 * -# Implement the fundamental callback methods (see \ref PRESOL_FUNDAMENTALCALLBACKS).
 * -# Implement the additional callback methods (see \ref PRESOL_ADDITIONALCALLBACKS). This is optional.
 *
 *
 * @section PRESOL_PROPERTIES Properties of a Presolver
 *
 * At the top of the new file "presol_mypresolver.c", you can find the presolver properties.
 * These are given as compiler defines.
 * In the C++ wrapper class, you have to provide the presolver properties by calling the constructor
 * of the abstract base class scip::ObjPresol from within your constructor.
 * The properties you have to set have the following meaning:
 *
 * \par PRESOL_NAME: the name of the presolver.
 * This name is used in the interactive shell to address the presolver.
 * Additionally, if you are searching for a presolver with SCIPfindPresol(), this name is looked up.
 * Names have to be <b>unique</b>: no two presolvers may have the same name.
 *
 * \par PRESOL_DESC: the description of the presolver.
 * This string is printed as a description of the presolver in the interactive shell.
 *
 * \par PRESOL_TIMING: the default timing of the presolver.
 * There are three presolving timings: FAST, MEDIUM, and EXHAUSTIVE.
 * Every presolving round starts with the FAST presolvers. MEDIUM presolvers are only called, if FAST presolvers did not find
 * enough reductions in this round so far, and EXHAUSTIVE presolving steps are only performed if all presolvers called before
 * in this round were unsuccessful.
 * Presolvers should be assigned a timing based on how expensive they are, e.g., presolvers that provide fast algorithms that
 * usually have a high impact (i.e., remove lots of variables or tighten bounds of many variables) should have a timing FAST.
 * If a presolver implements different algorithms of different complexity, it may also get multiple timings and check the timing
 * internally in the \ref PRESOLEXEC callback to decide which algorithms to run.
 *
 * \par PRESOL_PRIORITY: the priority of the presolver.
 * Within a presolving round, when calling all presolvers and presolving methods of propagators and constraint handlers
 * with a given timing, those are called in
 * a predefined order, which is given by the priorities of the presolvers and the check priorities of the
 * constraint handlers, see \ref CONS_PROPERTIES.
 * First, the presolvers with non-negative priority are called in the order of decreasing priority.
 * Next, the presolving methods of the different constraint handlers are called in the order of decreasing check
 * priority.
 * Finally, the presolvers with negative priority are called in the order of decreasing priority.
 * \n
 * Again, presolvers that provide fast algorithms that  usually have a high impact (i.e., remove lots of variables or tighten
 * bounds of many variables) should have a high priority.
 * An easy way to list the timings and
 * priorities of all presolvers, propagators, and constraint handlers is to type "display presolvers", "display propagators",
 * and "display conshdlrs" in the interactive shell of SCIP.
 *
 * \par PRESOL_MAXROUNDS: the default maximal number of rounds the presolver participates in.
 * The presolving is conducted in rounds: the presolvers and presolving methods of the constraint handlers
 * are called iteratively until no more reductions have been found or some other abort criterion applies.
 * The "maxrounds" parameter of a presolver imposes a limit on the number of presolving rounds in which the
 * presolver is called. The PRESOL_MAXROUNDS property specifies the default value for this parameter.
 * A value of -1 represents an unlimited number of rounds.
 *
 *
 * @section PRESOL_DATA Presolver Data
 *
 * Below the header "Data structures" you can find a struct which is called "struct SCIP_PresolData".
 * In this data structure, you can store the data of your presolver. For example, you should store the adjustable parameters
 * of the presolver in this data structure.
 * If you are using C++, you can add presolver data as usual as object variables to your class.
 * \n
 * Defining presolver data is optional. You can leave this struct empty.
 *
 *
 * @section PRESOL_INTERFACE Interface Methods
 *
 * At the bottom of "presol_mypresolver.c", you can find the interface method SCIPincludePresolMypresolver(),
 * which also appears in "presol_mypresolver.h"
 * SCIPincludePresolMypresolver() is called by the user, if (s)he wants to include the presolver,
 * i.e., if (s)he wants to use the presolver in his/her application.
 *
 * This method only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the presolver. For this, you can either call SCIPincludePresol(),
 * or SCIPincludePresolBasic() since SCIP version 3.0. In the latter variant, \ref PRESOL_ADDITIONALCALLBACKS "additional callbacks"
 * must be added via setter functions as, e.g., SCIPsetPresolCopy(). We recommend this latter variant because
 * it is more stable towards future SCIP versions which might have more callbacks, whereas source code using the first
 * variant must be manually adjusted with every SCIP release containing new callbacks for presolvers in order to compile.
 *
 * If you are using presolver data, you have to allocate the memory for the data at this point.
 * You can do this by calling:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &presoldata) );
 * \endcode
 * You also have to initialize the fields in struct SCIP_PresolData afterwards. For freeing the
 * presolver data, see \ref PRESOLFREE.
 *
 * You may also add user parameters for your presolver, see \ref PARAM for how to add user parameters and
 * the method SCIPincludePresolTrivial() in src/scip/presol_trivial.c for an example.
 *
 *
 * @section PRESOL_FUNDAMENTALCALLBACKS Fundamental Callback Methods of a Presolver
 *
 * The fundamental callback methods of the plugins are the ones that have to be implemented in order to obtain
 * an operational algorithm.
 * They are passed together with the presolver itself to SCIP using SCIPincludePresol() or SCIPincludePresolBasic(),
 * see @ref PRESOL_INTERFACE.
 *
 * Presolver plugins have only one fundamental callback method, namely the @ref PRESOLEXEC method.
 * This method has to be implemented for every presolver; the other callback methods are optional.
 * In the C++ wrapper class scip::ObjPresol, the scip_exec() method (which corresponds to the PRESOLEXEC callback) is a virtual
 * abstract member function.
 * You have to implement it in order to be able to construct an object of your presolver class.
 *
 * Additional documentation for the callback methods, in particular to their input parameters,
 * can be found in type_presol.h.
 *
 * @subsection PRESOLEXEC
 *
 * The PRESOLEXEC callback is called inside the presolving loop and should perform the actual presolving reductions.
 * It should inspect the problem instance at hand and simplify it by tightening bounds of variables, aggregating or fixing
 * variables, changing the type of variables, modifying the graph that represents the instance of your application, and
 * the like.
 *
 * Typical methods called by a presolver are, for example, SCIPchgVarType(), SCIPfixVar(), SCIPaggregateVars(), SCIPtightenVarLb(),
 * and SCIPtightenVarUb().
 *
 *
 * @section PRESOL_ADDITIONALCALLBACKS Additional Callback Methods of a Presolver
 *
 * The additional callback methods do not need to be implemented in every case. However, some of them have to be
 * implemented for most applications, they can be used, for example, to initialize and free private data.
 * Additional callbacks can either be passed directly with SCIPincludePresol() to SCIP or via specific
 * <b>setter functions</b> after a call of SCIPincludePresolBasic(), see also @ref PRESOL_INTERFACE.
 *
 * @subsection PRESOLFREE
 *
 * If you are using presolver data (see \ref PRESOL_DATA and \ref PRESOL_INTERFACE), you have to implement this method in order to free the presolver data.
 * This can be done by the following procedure:
 *
 * @refsnippet{src/scip/presol_boundshift.c,SnippetPresolFreeBoundshift}
 *
 * If you have allocated memory for fields in your presolver data, remember to free this memory
 * before freeing the presolver data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 * @subsection PRESOLINIT
 *
 * The PRESOLINIT callback is executed after the problem is transformed.
 * The presolver may, e.g., use this call to initialize its presolver data.
 * The difference between the original and the transformed problem is explained in
 * "What is this thing with the original and the transformed problem about?" on \ref FAQ.
 *
 * @subsection PRESOLCOPY
 *
 * The PRESOLCOPY callback is executed when a SCIP instance is copied, e.g. to
 * solve a sub-SCIP. By
 * defining this callback as
 * <code>NULL</code> the user disables the execution of the specified
 * presolver for all copied SCIP instances. This may deteriorate the performance
 * of primal heuristics using sub-SCIPs.
 *
 * @subsection PRESOLEXIT
 *
 * The PRESOLEXIT callback is executed before the transformed problem is freed.
 * In this method, the presolver should free all resources that have been allocated for the solving process in PRESOLINIT.
 *
 * @subsection PRESOLINITPRE
 *
 * The PRESOLINITPRE callback is executed when the presolving is about to begin.
 * The presolver may use this call to initialize its presolving data which only need to exist during the presolving stage.
 *
 * @subsection PRESOLEXITPRE
 *
 * The PRESOLEXITPRE callback is executed after presolving finishes and before the branch-and-bound process begins.
 * The presolver should use this call to clean up its presolving data, which was allocated in PRESOLINITPRE.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page SEPA How to add separators
 *
 * Separators are used to generate cutting planes that strengthen the LP relaxation of the problem formulation, but are
 * not required for a completeness and correctness of the model.
 * In contrast, constraint-based cutting planes, the second type of cutting planes in SCIP, are separated in the CONSSEPALP and
 * CONSSEPASOL callback methods of the constraint handlers, see \ref CONSSEPALP and \ref CONSSEPASOL. These cuts are
 * valid inequalities or even facets of the polyhedron described by a single constraint or a subset of the constraints of
 * a single constraint class. See also
 * "When should I implement a constraint handler, when should I implement a separator?" on \ref FAQ.
 * \n
 * A complete list of all separators contained in this release can be found \ref SEPARATORS "here".
 *
 * We now explain how users can add their own separators.
 * Take the separator for the class of Gomory mixed integer inequalities (src/scip/sepa_gomory.c) as an example.
 * As all other default plugins, it is written in C. C++ users can easily adapt the code by using the scip::ObjSepa wrapper
 * base class and implement the scip_...() virtual methods instead of the SCIP_DECL_SEPA... callback methods.
 *
 * Additional documentation for the callback methods of a separator, in particular for the input parameters,
 * can be found in the file type_sepa.h.
 *
 * Here is what you have to do to implement a separator:
 * -# Copy the template files src/scip/sepa_xyz.c and src/scip/sepa_xyz.h into files "sepa_myseparator.c"
 *    and "sepa_myseparator.h".
      \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPincludeSepaMyseparator() in order to include the separator into your SCIP instance,
 *    e.g., in the main file of your project (see, e.g., src/cmain.c in the Binpacking example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "myseparator".
 * -# Adjust the properties of the separator (see \ref SEPA_PROPERTIES).
 * -# Define the separator data (see \ref SEPA_DATA). This is optional.
 * -# Implement the interface methods (see \ref SEPA_INTERFACE).
 * -# Implement the fundamental callback methods (see \ref SEPA_FUNDAMENTALCALLBACKS).
 * -# Implement the additional callback methods (see \ref SEPA_ADDITIONALCALLBACKS).  This is optional.
 *
 *
 * @section SEPA_PROPERTIES Properties of a Separator
 *
 * At the top of the new file "sepa_myseparator.c", you can find the separator properties.
 * These are given as compiler defines.
 * In the C++ wrapper class, you have to provide the separator properties by calling the constructor
 * of the abstract base class scip::ObjSepa from within your constructor.
 * The properties you have to set have the following meaning:
 *
 * \par SEPA_NAME: the name of the separator.
 * This name is used in the interactive shell to address the separator.
 * Additionally, if you are searching for a separator with SCIPfindSepa(), this name is looked up.
 * Names have to be unique: no two separators may have the same name.
 *
 * \par SEPA_DESC: the description of the separator.
 * This string is printed as a description of the separator in the interactive shell.
 *
 * \par SEPA_PRIORITY: the priority of the separator.
 * In each separation round during the price-and-cut loop of the subproblem processing or the separation loop
 * of the primal solution separation, the separators and separation methods of the constraint handlers are called in
 * a predefined order, which is given by the priorities of the separators and the separation priorities
 * of the constraint handlers (see \ref CONS_PROPERTIES).
 * First, the separators with non-negative priority are called in the order of decreasing priority.
 * Next, the separation methods of the constraint handlers are called in the order of decreasing separation
 * priority.
 * Finally, the separators with negative priority are called in the order of decreasing priority. An easy way to list the
 * priorities of all separators and constraint handlers is to type "display separators" and "display conshdlrs" in
 * the interactive shell.
 * \n
 * The priority of the separator should be set according to the complexity of the cut separation algorithm and the
 * impact of the resulting cuts: separators that provide fast algorithms that usually have a high impact (i.e., cut off
 * a large portion of the LP relaxation) should have a high priority.
 * See \ref SEPAEXECLP and \ref SEPAEXECSOL for further details of the separation callbacks.
 *
 * \par SEPA_FREQ: the default frequency for separating cuts.
 * The frequency defines the depth levels at which the separation methods \ref SEPAEXECLP and \ref SEPAEXECSOL are called.
 * For example, a frequency of 7 means, that the separation callback is executed for subproblems that are in depth
 * 0, 7, 14, ... of the branching tree. A frequency of 0 means, that the separation method is only called at the root node.
 * A frequency of -1 disables the separator.
 * \n
 * The frequency can be adjusted by the user. This property of the separator only defines the default value of the frequency.
 * If you want to have a more flexible control of when to execute the separation algorithm, you have to assign
 * a frequency of 1 and implement a check at the beginning of your separation methods whether you really want to execute
 * the separation or not. If you do not want to execute it, set the result code of
 * \ref SEPAEXECLP and \ref SEPAEXECSOL to SCIP_DIDNOTRUN.
 *
 * \par SEPA_MAXBOUNDDIST: the default maximal relative distance from the current node's dual bound to primal bound compared to best node's dual bound for applying separation.
 * At the current branch-and-bound node, the relative distance from its dual bound (local dual bound)
 * to the primal bound compared to the best node's dual bound (global dual bound) is considered. The separation method
 * of the separator will only be applied at the current node if this relative distance does not exceed SEPA_MAXBOUNDDIST.
 * \n
 * For example, if the global dual bound is 50 and the primal bound is 60, SEPA_MAXBOUNDDIST = 0.25 means that separation
 * is only applied if the current node's dual bound is in the first quarter of the interval [50,60], i.e., if it is less
 * than or equal to 52.5.
 * \n
 * In particular, the values 0.0 and 1.0 mean that separation is applied at the current best node only or at all
 * nodes, respectively. Since separation seems to be most important to apply at nodes that define to the global
 * dual bound, 0.0 is probably a good choice for SEPA_MAXBOUNDDIST.
 * Note that separators with a frequency of SEPA_FREQ = 0 are only applied at the root node.
 * Obviously, at the root node the local dual bound is equal to the global dual bound and thus, the separator is called
 * for any value of SEPA_MAXBOUNDDIST.
 *
 * \par SEPA_USESSUBSCIP: Does the separator use a secondary SCIP instance?
 * Some heuristics and separators solve MIPs or SAT problems and use a secondary SCIP instance. Examples are
 * Large Neighborhood Search heuristics such as RINS and Local Branching or the CGMIP separator. To avoid recursion,
 * these plugins usually deactivate all other plugins that solve MIPs. If a separator uses a secondary SCIP instance,
 * this parameter has to be TRUE and it is recommended to call SCIPsetSubscipsOff() for the secondary SCIP instance.
 *
 * \par SEPA_DELAY: the default for whether the separation method should be delayed, if other separators or constraint handlers found cuts.
 * If the separator's separation method is marked to be delayed, it is only executed after no other separator
 * or constraint handler found a cut during the price-and-cut loop.
 * If the separation method of the separator is very expensive, you may want to mark it to be delayed until all cheap
 * separation methods have been executed.
 *
 * @section SEPA_DATA Separator Data
 *
 * Below the header "Data structures" you can find a struct which is called "struct SCIP_SepaData".
 * In this data structure, you can store the data of your separator. For example, you should store the adjustable
 * parameters of the separator in this data structure. In a separator, user parameters for the maximal number of
 * separation rounds per node and for the maximal number of cuts separated per separation round might be useful.
 * If you are using C++, you can add separator data as usual as object variables to your class.
 * \n
 * Defining separator data is optional. You can leave the struct empty.
 *
 * @section SEPA_INTERFACE Interface Methods
 *
 * At the bottom of "sepa_myseparator.c", you can find the interface method SCIPincludeSepaMyseparator(),
 * which also appears in "sepa_myseparator.h"
 * SCIPincludeSepaMyseparator() is called by the user, if (s)he wants to include the separator,
 * i.e., if (s)he wants to use the separator in his/her application.
 *
 * This method only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the separator. For this, you can either call SCIPincludeSepa(),
 * or SCIPincludeSepaBasic() since SCIP version 3.0. In the latter variant, \ref SEPA_ADDITIONALCALLBACKS "additional callbacks"
 * must be added via setter functions as, e.g., SCIPsetSepaCopy(). We recommend this latter variant because
 * it is more stable towards future SCIP versions which might have more callbacks, whereas source code using the first
 * variant must be manually adjusted with every SCIP release containing new callbacks for separators in order to compile.
 *
 * If you are using separator data, you have to allocate the memory
 * for the data at this point. You can do this by calling:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &sepadata) );
 * \endcode
 * You also have to initialize the fields in "struct SCIP_SepaData" afterwards. For freeing the
 * separator data, see \ref SEPAFREE.
 *
 * You may also add user parameters for your separator, see \ref PARAM for how to add user parameters and
 * the method SCIPincludeSepaGomory() in src/scip/sepa_gomory.c for an example.
 *
 *
 * @section SEPA_FUNDAMENTALCALLBACKS Fundamental Callback Methods of a Separator
 *
 * The fundamental callback methods of the plugins are the ones that have to be implemented in order to obtain
 * an operational algorithm.
 * They are passed together with the separator itself to SCIP using SCIPincludeSepa() or SCIPincludeSepaBasic(),
 * see @ref SEPA_INTERFACE.
 *
 * Separator plugins have two callbacks, @ref SEPAEXECLP and @ref SEPAEXECSOL, of which at least one must be implemented.
 *
 * Additional documentation for the callback methods, in particular to their input parameters,
 * can be found in type_sepa.h.
 *
 * @subsection SEPAEXECLP
 *
 * The SEPAEXECLP callback is executed during the price-and-cut loop of the subproblem processing.
 * It should try to generate general purpose cutting planes in order to separate the current LP solution.
 * The method is called in the LP solution loop, which means that a valid LP solution exists.
 *
 * Usually, the callback searches and produces cuts, that are added with a call to SCIPaddCut().
 * If the cut should be added to the global cut pool, it calls SCIPaddPoolCut().
 * In addition to LP rows, the callback may also produce domain reductions or add additional constraints.
 *
 * Overall, the SEPAEXECLP callback has the following options, which is indicated by the possible return values of
 * the 'result' variable (see type_sepa.h):
 *  - detecting that the node is infeasible in the variable's bounds and can be cut off (result SCIP_CUTOFF)
 *  - adding an additional constraint (result SCIP_CONSADDED)
 *  - reducing a variable's domain (result SCIP_REDUCEDDOM)
 *  - adding a cutting plane to the LP (result SCIP_SEPARATED)
 *  - stating that the separator searched, but did not find domain reductions, cutting planes, or cut constraints
 *    (result SCIP_DIDNOTFIND)
 *  - stating that the separator was skipped (result SCIP_DIDNOTRUN)
 *  - stating that the separator was skipped, but should be called again (result SCIP_DELAYED)
 *  - stating that a new separation round should be started without calling the remaining separator methods (result SCIP_NEWROUND)
 *
 * @subsection SEPAEXECSOL
 *
 * The SEPAEXECSOL callback is executed during the separation loop on arbitrary primal solutions.
 * It should try to generate general purpose cutting planes in order to separate the given primal solution.
 * The method is not called in the LP solution loop, which means that there is no valid LP solution.
 *
 * In the standard SCIP environment, the SEPAEXECSOL callback is not used because only LP solutions are
 * separated. The SEPAEXECSOL callback provides means to support external relaxation handlers like semidefinite
 * relaxations that want to separate an intermediate primal solution vector. Thus, if you do not want to support
 * such external plugins, you do not need to implement this callback method.
 *
 * Usually, the callback searches and produces cuts, that are added with a call to SCIPaddCut().
 * If the cut should be added to the global cut pool, it calls SCIPaddPoolCut().
 * In addition to LP rows, the callback may also produce domain reductions or add other constraints.
 *
 * Overall, the SEPAEXECSOL callback has the following options, which is indicated by the possible return values of
 * the 'result' variable (see type_sepa.h):
 *  - detecting that the node is infeasible in the variable's bounds and can be cut off (result SCIP_CUTOFF)
 *  - adding an additional constraint (result SCIP_CONSADDED)
 *  - reducing a variable's domain (result SCIP_REDUCEDDOM)
 *  - adding a cutting plane to the LP (result SCIP_SEPARATED)
 *  - stating that the separator searched, but did not find domain reductions, cutting planes, or cut constraints
 *    (result SCIP_DIDNOTFIND)
 *  - stating that the separator was skipped (result SCIP_DIDNOTRUN)
 *  - stating that the separator was skipped, but should be called again (result SCIP_DELAYED)
 *  - stating that a new separation round should be started without calling the remaining separator methods (result SCIP_NEWROUND)
 *
 *
 * @section SEPA_ADDITIONALCALLBACKS Additional Callback Methods of a Separator
 *
 * The additional callback methods do not need to be implemented in every case. However, some of them have to be
 * implemented for most applications, they can be used, for example, to initialize and free private data.
 * Additional callbacks can either be passed directly with SCIPincludeSepa() to SCIP or via specific
 * <b>setter functions</b> after a call of SCIPincludeSepaBasic(), see also @ref SEPA_INTERFACE.
 *
 * @subsection SEPAFREE
 *
 * If you are using separator data (see \ref SEPA_DATA and \ref SEPA_INTERFACE), you have to implement this method
 * in order to free the separator data. This can be done by the following procedure:
 *
 * @refsnippet{src/scip/sepa_gomory.c,SnippetSepaFreeGomory}
 *
 * If you have allocated memory for fields in your separator data, remember to free this memory
 * before freeing the separator data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 * @subsection SEPACOPY
 *
 * The SEPACOPY callback is executed when a SCIP instance is copied, e.g. to
 * solve a sub-SCIP. By
 * defining this callback as
 * <code>NULL</code> the user disables the execution of the specified
 * separator for all copied SCIP instances. This may deteriorate the performance
 * of primal heuristics using sub-SCIPs.
 *
 * @subsection SEPAINIT
 *
 * The SEPAINIT callback is executed after the problem is transformed.
 * The separator may, e.g., use this call to initialize its separator data.
 * The difference between the original and the transformed problem is explained in
 * "What is this thing with the original and the transformed problem about?" on \ref FAQ.
 *
 * @subsection SEPAEXIT
 *
 * The SEPAEXIT callback is executed before the transformed problem is freed.
 * In this method, the separator should free all resources that have been allocated for the solving process in SEPAINIT.
 *
 * @subsection SEPAINITSOL
 *
 * The SEPAINITSOL callback is executed when the presolving is finished and the branch-and-bound process is about to
 * begin. The separator may use this call to initialize its branch-and-bound specific data.
 *
 * @subsection SEPAEXITSOL
 *
 * The SEPAEXITSOL callback is executed before the branch-and-bound process is freed. The separator should use this call
 * to clean up its branch-and-bound data, in particular to release all LP rows that it has created or captured.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page PROP How to add propagators
 *
 * Propagators are used to tighten the domains of the variables. Like for cutting planes, there are two different types
 * of domain propagations. Constraint based (primal) domain propagation algorithms are part of the corresponding
 * constraint handlers, see \ref CONSPROP. In contrast, domain propagators usually provide dual propagations, i.e.,
 * propagations that can be applied using the objective function and the current best known primal solution. This
 * section deals with such propagators.
 *
 * A complete list of all propagators contained in this release can be found \ref PROPAGATORS "here".
 *
 * We now explain how users can add their own propagators.  Take the pseudo objective function propagator
 * (src/scip/prop_pseudoobj.c) as an example.  As all other default plugins, it is written in C. C++ users can easily
 * adapt the code by using the scip::ObjProp wrapper base class and implement the @c scip_...() virtual methods instead
 * of the @c SCIP_DECL_PROP... callback methods.
 *
 * Additional documentation for the callback methods of a propagator can be found in the file type_prop.h.
 *
 * Here is what you have to do to implement a propagator:
 * -# Copy the template files src/scip/prop_xyz.c and src/scip/prop_xyz.h into files named "prop_mypropagator.c"
 *    and "prop_mypropagator.h".
 *    \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPincludePropMypropagator() in order to include the propagator into your SCIP instance,
 *    e.g., in the main file of your project (see, e.g., src/cmain.c in the Binpacking example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "mypropagator".
 * -# Adjust the properties of the propagator (see \ref PROP_PROPERTIES).
 * -# Define the propagator data (see \ref PROP_DATA). This is optional.
 * -# Implement the interface methods (see \ref PROP_INTERFACE).
 * -# Implement the fundamental callback methods (see \ref PROP_FUNDAMENTALCALLBACKS).
 * -# Implement the additional callback methods (see \ref PROP_ADDITIONALCALLBACKS). This is optional.
 *
 * @section PROP_PROPERTIES Properties of a Propagator
 *
 * At the top of the new file "prop_mypropagator.c" you can find the propagator properties. These are given as compiler
 * defines. The presolving-related properties are optional,
 * they only have to be defined if the propagator supports presolving routines.
 * In the C++ wrapper class, you have to provide the propagator properties by calling the constructor of the
 * abstract base class scip::ObjProp from within your constructor.  The properties you have the following meaning:
 *
 * @subsection PROP_FUNDAMENTALPROPERTIES Fundamental properties of a propagator
 *
 * \par PROP_NAME: the name of the propagator.
 * This name is used in the interactive shell to address the propagator.  Additionally, if you are searching for a
 * propagator with SCIPfindProp(), this name is searched for.  Names have to be unique: no two propagators may have the
 * same name.
 *
 * \par PROP_DESC: the description of the propagator.
 * This string is printed as a description of the propagator in the interactive shell.
 *
 * \par PROP_PRIORITY: the priority of the propagator.
 * In each propagation round, the propagators and propagation methods of the constraint handlers are called in a
 * predefined order, which is given by the priorities of the propagators and the check priorities of the constraint
 * handlers.  First, the propagators with non-negative priority are called in order of decreasing priority.  Next, the
 * propagation methods of the different constraint handlers are called in order of decreasing check priority.  Finally,
 * the propagators with negative priority are called in order of decreasing priority.  \n The priority of the
 * propagators should be set according to the complexity of the propagation algorithm and the impact of the domain
 * propagations: propagators providing fast algorithms that usually have a high impact (i.e., tighten many bounds)
 * should have a high priority.
 *
 * \par PROP_FREQ: the default frequency for propagating domains.
 * The frequency defines the depth levels at which the propagation method \ref PROPEXEC is called.  For example, a
 * frequency of 7 means, that the propagation callback is executed for subproblems that are in depth 0, 7, 14, ... of
 * the branching tree. A frequency of 0 means that propagation is only applied in preprocessing and at the root node. A
 * frequency of -1 disables the propagator.
 * \n
 * The frequency can be adjusted by the user. This property of the propagator only defines the default value of the
 * frequency.\n
 * <b>Note:</b> If you want to have a more flexible control of when to execute the propagation algorithm, you have to
 * assign a frequency of 1 and implement a check at the beginning of your propagation algorithm whether you really want
 * to execute the domain propagation or not. If you do not want to execute it, set the result code to SCIP_DIDNOTRUN.
 *
 * \par PROP_DELAY: the default for whether the propagation method should be delayed, if other propagators or constraint handlers found domain reductions.
 * If the propagator's propagation method is marked to be delayed, it is only executed after no other propagator or
 * constraint handler found a domain reduction in the current iteration of the domain propagation loop.  If the
 * propagation method of the propagator is very expensive, you may want to mark it to be delayed until all cheap
 * propagation methods have been executed.
 *
 * \par PROP_TIMING: the timing mask of the propagator.
 * SCIP calls the domain propagation routines at different places in the node processing loop.
 * This property indicates at which places the propagator is called.
 * Possible values are defined in type_timing.h and can be concatenated, e.g., as in SCIP_PROPTIMING_ALWAYS.
 *
 * @subsection PROP_ADDITIONALPROPERTIES Optional propagator properties
 *
 * The following properties are optional and only need to be defined if the propagator supports
 * presolving, that is, if the \ref PROPPRESOL "presolving callback" is implemented.

 * \par PROP_PRESOLTIMING: the timing of the presolving method (FAST, MEDIUM, or EXHAUSTIVE).
 * Every presolving round starts with the FAST presolving methods. MEDIUM presolvers are only called, if FAST presolvers did not find
 * enough reductions in this round so far, and EXHAUSTIVE presolving steps are only performed if all presolvers called before
 * in this round were unsuccessful.
 * Presolving methods should be assigned a timing based on how expensive they are, e.g., presolvers that provide fast algorithms that
 * usually have a high impact (i.e., remove lots of variables or tighten bounds of many variables) should have a timing FAST.
 * If a presolving method implements different algorithms of different complexity, it may also get multiple timings and check the timing
 * internally in the \ref PROPPRESOL callback to decide which algorithms to run.
 *
 * \par PROP_PRESOL_PRIORITY: the priority of the presolving method.
 * This attribute is analogous to the PROP_PRIORITY flag, but deals with the preprocessing method of the presolver.
 *
 * \par PROP_PRESOL_MAXROUNDS: the default maximal number of presolving rounds the propagator participates in.
 * The preprocessing is executed in rounds.
 * If enough changes have been applied to the model, an additional preprocessing round is performed.
 * The MAXROUNDS parameter of a propagator denotes the maximal number of preprocessing rounds, the propagator
 * participates in.
 * A value of -1 means, that there is no limit on the number of rounds.
 * A value of 0 means, the preprocessing callback of the propagator is disabled.
 *
 * @section PROP_DATA Propagator Data
 *
 * Below the title "Data structures" you can find a struct called <code>struct SCIP_PropData</code>.  In this data
 * structure, you can store the data of your propagator. For example, you should store the adjustable parameters of the
 * propagator in this data structure.  If you are using C++, you can add propagator data as object variables to your
 * class as usual .
 * \n
 * Defining propagator data is optional. You can leave the struct empty.
 *
 *
 * @section PROP_INTERFACE Interface Methods
 *
 * At the bottom of "prop_mypropagator.c", you can find the interface method SCIPincludeSepaMypropagator(),
 * which also appears in "prop_mypropagator.h"
 * SCIPincludePropMypropagator() is called by the user, if (s)he wants to include the propagator,
 * i.e., if (s)he wants to use the propagator in his/her application.
 *
 * This method only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the propagator. For this, you can either call SCIPincludeProp(),
 * or SCIPincludePropBasic() since SCIP version 3.0. In the latter variant, \ref PROP_ADDITIONALCALLBACKS "additional callbacks"
 * must be added via setter functions as, e.g., SCIPsetPropCopy(). We recommend this latter variant because
 * it is more stable towards future SCIP versions which might have more callbacks, whereas source code using the first
 * variant must be manually adjusted with every SCIP release containing new callbacks for separators in order to compile.
 *
 *
 * If you are using propagator data, you have to allocate the memory for the data at this point.  You can do this by
 * calling
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &propdata) );
 * \endcode
 * You also have to initialize the fields in <code>struct SCIP_PropData</code> afterwards.
 *
 * You may also add user parameters for your propagator, see the method SCIPincludePropPseudoobj() in
 * src/scip/prop_pseudoobj.c for an example.
 *
 *
 * @section PROP_FUNDAMENTALCALLBACKS Fundamental Callback Methods of a Propagator
 *
 * The fundamental callback methods of the plugins are the ones that have to be implemented in order to obtain
 * an operational algorithm.
 * They are passed together with the propagator itself to SCIP using SCIPincludeProp() or SCIPincludePropBasic(),
 * see @ref PROP_INTERFACE.
 *
 * Propagator plugins have one fundamental callback method, namely the \ref PROPEXEC method
 * method.  This method has to be implemented for every propagator; the other callback methods are optional.  In the
 * C++ wrapper class scip::ObjProp, the scip_exec() method (which corresponds to the \ref PROPEXEC
 * callback) is a virtual abstract member function. You have to
 * implement it in order to be able to construct an object of your propagator class.
 *
 * Additional documentation for the callback methods can be found in type_prop.h.
 *
 * @subsection PROPEXEC
 *
 * The PROPEXEC callback is called during presolving and during the subproblem processing. It should perform the actual
 * domain propagation, which means that it should tighten the variables' bounds.  The technique of domain propagation,
 * which is the main workhorse of constraint programming, is called "node preprocessing" in the Integer Programming
 * community.
 *
 * The PROPEXEC callback has the following options:
 *  - detecting that the node is infeasible in the variables' bounds and can be cut off (result SCIP_CUTOFF)
 *  - reducing (i.e, tightening) the domains of some variables (result SCIP_REDUCEDDOM)
 *  - stating that the propagator searched, but did not find domain reductions, cutting planes, or cut constraints
 *    (result SCIP_DIDNOTFIND)
 *  - stating that the propagator was skipped (result SCIP_DIDNOTRUN)
 *  - stating that the propagator was skipped, but should be called again (result SCIP_DELAYED)
 *
 *
 *
 * @section PROP_ADDITIONALCALLBACKS Additional Callback Methods of a Propagator
 *
 * The additional callback methods do not need to be implemented in every case. However, some of them have to be
 * implemented for most applications, they can be used, for example, to initialize and free private data.
 * Additional callbacks can either be passed directly with SCIPincludeProp() to SCIP or via specific
 * <b>setter functions</b> after a call of SCIPincludePropBasic(), see also @ref PROP_INTERFACE.
 *
 * @subsection PROPRESPROP
 *
 * If the propagator wants to support \ref CONF "conflict analysis", it has to supply the PROPRESPROP method.  It also should call
 * SCIPinferVarLbProp() or SCIPinferVarUbProp() in the domain propagation instead of SCIPchgVarLb() or SCIPchgVarUb() in
 * order to deduce bound changes on variables.  In the SCIPinferVarLbProp() and SCIPinferVarUbProp() calls, the
 * propagator provides a pointer to itself and an integer value "inferinfo" that can be arbitrarily chosen.
 *
 * The propagation conflict resolving method PROPRESPROP must then be implemented to provide the "reasons" for the bound
 * changes, i.e., the bounds of variables at the time of the propagation, which forced the propagator to set the
 * conflict variable's bound to its current value. It can use the "inferinfo" tag to identify its own propagation rule
 * and thus identify the "reason" bounds. The bounds that form the reason of the assignment must then be provided by
 * calls to SCIPaddConflictLb() and SCIPaddConflictUb() in the propagation conflict resolving method.
 *
 * See the description of the propagation conflict resolving method \ref CONSRESPROP of constraint handlers for
 * further details.
 *
 * Omitting the PROPRESPROP callback circumvents the implementation of the usually rather complex conflict resolving method.
 * Yet, it
 * will make the conflict analysis less effective. We suggest to first omit the conflict resolving method and check how
 * effective the propagation method is. If it produces a lot of propagations for your application, you definitely should
 * consider implementing the conflict resolving method.
 *
 *
 * @subsection PROPFREE
 *
 * If you are using propagator data, you have to implement this method in order to free the propagator data.
 * This can be done by the following procedure:
 *
 * @refsnippet{src/scip/prop_redcost.c,SnippetPropFreeRedcost}
 *
 * If you have allocated memory for fields in your propagator data, remember to free this memory
 * before freeing the propagator data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 * @subsection PROPINIT
 *
 * The PROPINIT callback is executed after the problem is transformed.  The propagator may, e.g., use this call to
 * initialize its propagator data.
 *
 * @subsection PROPCOPY
 *
 * The PROPCOPY callback is executed when a SCIP instance is copied, e.g. to
 * solve a sub-SCIP. By
 * defining this callback as
 * <code>NULL</code> the user disables the execution of the specified
 * propagator for all copied SCIP instances. This may deteriorate the performance
 * of primal heuristics using sub-SCIPs.
 *
 * @subsection PROPEXIT
 *
 * The PROPEXIT callback is executed before the transformed problem is freed.
 * In this method, the propagator should free all resources that have been allocated for the solving process in PROPINIT.
 *
 * @subsection PROPINITPRE
 *
 * The PROPINITPRE callback is executed before the preprocessing is started, even if presolving is turned off.
 * The propagator may use this call to initialize its presolving data before the presolving process begins.
 *
 * @subsection PROPEXITPRE
 *
 * The PROPEXITPRE callback is executed after the preprocessing has been finished, even if presolving is turned off.
 * The propagator may use this call, e.g., to clean up its presolving data.
 * Besides clean up, no time consuming operations should be done.
 *
 * @subsection PROPINITSOL
 *
 * The PROPINITSOL callback is executed when the presolving is finished and the branch-and-bound process is about to
 * begin.
 * The propagator may use this call to initialize its branch-and-bound specific data.
 *
 * @subsection PROPEXITSOL
 *
 * The PROPEXITSOL callback is executed before the branch-and-bound process is freed.
 * The propagator should use this call to clean up its branch-and-bound data.
 *
 * @subsection PROPPRESOL
 *
 * Seaches for domain propagations, analogous to the \ref PROPEXEC callback.
 * However, this callback is called during preprocessing.
 *
 * To inform SCIP that the presolving method found a reduction the result pointer has to be set in a proper way.
 * The following options are possible:
 *
 *  - SCIP_UNBOUNDED  : at least one variable is not bounded by any constraint in objective direction
 *  - SCIP_CUTOFF     : at least one domain reduction that renders the problem infeasible has been found
 *  - SCIP_SUCCESS    : the presolver found a domain reduction
 *  - SCIP_DIDNOTFIND : the presolver searched, but did not find a presolving change
 *  - SCIP_DIDNOTRUN  : the presolver was skipped
 *  - SCIP_DELAYED    : the presolver was skipped, but should be called again
 *
 *
 * Please see also the @ref PROP_ADDITIONALPROPERTIES section to learn about the properties
 * PROP_PRESOLTIMING and PROP_PRESOL_MAXROUNDS, which influence the behaviour of SCIP
 * calling PROPPRESOL.
 *
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page BRANCH How to add branching rules
 *
 * Branching rules are used to split the problem at the current node into smaller subproblems. Branching rules can be called at three
 * different occasions, which is why they have three different execution methods (see \ref
 * BRANCHRULE_ADDITIONALCALLBACKS).  Branching is performed if:
 * - the LP solution of the current problem is fractional. In this case, the integrality constraint handler calls the
 *   \ref BRANCHEXECLP methods of the branching rules.
 * - the list of external branching candidates is not empty. This will only be the case if branching candidates were added
 *   by a user's \ref RELAX "relaxation handler" or \ref CONS "constraint handler" plugin, calling SCIPaddExternBranchCand().
 *   These branching candidates should be processed by the \ref BRANCHEXECEXT method.
 * - if an integral solution violates one or more constraints and this infeasibility could not be resolved in the callback methods
 *   \ref CONSENFOLP and \ref CONSENFOPS of the corresponding constraint handlers. In this case, the \ref BRANCHEXECPS method will be called. This is the
 *   standard case, if you use SCIP as a pure CP or SAT solver. If the LP or any other type of relaxation is used, then
 *   branching on pseudo solutions works as a last resort.
 *
 * The idea of branching rules is to take a global view on the problem. In contrast, branching paradigms which are
 * specific to one type of constraint are best implemented within the enforcement callbacks of your constraint handler.
 * See, e.g., the constraint specific branching rules provided by the constraint handlers for special ordered sets
 * (src/scip/cons_sos{1,2}.c)).
 * \n
 * All branching rules that come with the default distribution of SCIP create two subproblems by splitting a single
 * variable's domain.  It is, however, fully supported to implement much more general branching schemes, for example by
 * creating more than two subproblems, or by adding additional constraints to the subproblems instead of tightening the
 * domains of the variables.
 * \n
 * A complete list of all branching rules contained in this release can be found \ref BRANCHINGRULES "here".
 *
 * We now explain how users can add their own branching rules.  Take the most infeasible LP branching rule
 * (src/scip/branch_mostinf.c) as an example.  As all other default plugins, it is written in C. C++ users can easily
 * adapt the code by using the scip::ObjBranchrule wrapper base class and implement the scip_...() virtual methods instead of
 * the SCIP_DECL_BRANCH... callback methods.
 *
 * Additional documentation for the callback methods of a branching rule can be found in the file type_branch.h.
 *
 * Here is what you have to do to implement a branching rule:
 * -# Copy the template files src/scip/branch_xyz.c and src/scip/branch_xyz.h into files named
 *    "branch_mybranchingrule.c" and "branch_mybranchingrule.h".
 *    \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPincludeBranchruleMybranchingrule() in order to include the branching rule into your SCIP instance,
 *    e.g., in the main file of your project (see, e.g., src/cmain.c in the Binpacking example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "mybranchingrule".
 * -# Adjust the properties of the branching rule (see \ref BRANCHRULE_PROPERTIES).
 * -# Define the branching rule data (see \ref BRANCHRULE_DATA). This is optional.
 * -# Implement the interface methods (see \ref BRANCHRULE_INTERFACE).
 * -# Implement the fundamental callback methods (see \ref BRANCHRULE_FUNDAMENTALCALLBACKS).
 * -# Implement the additional callback methods (see \ref BRANCHRULE_ADDITIONALCALLBACKS). This is optional.
 *
 *
 * @section BRANCHRULE_PROPERTIES Properties of a Branching Rule
 *
 * At the top of the new file "branch_mybranchingrule.c" you can find the branching rule properties.
 * These are given as compiler defines.
 * In the C++ wrapper class, you have to provide the branching rule properties by calling the constructor
 * of the abstract base class scip::ObjBranchrule from within your constructor.
 * The properties you have to set have the following meaning:
 *
 * \par BRANCHRULE_NAME: the name of the branching rule.
 * This name is used in the interactive shell to address the branching rule.
 * Additionally, if you are searching for a branching rule with SCIPfindBranchrule(), this name is looked up.
 * Names have to be unique: no two branching rules may have the same name.
 *
 * \par BRANCHRULE_DESC: the description of the branching rule.
 * This string is printed as a description of the branching rule in the interactive shell.
 *
 * \par BRANCHRULE_PRIORITY: the default value for the priority of the branching rule.
 * In the subproblem processing, the branching rules are called in decreasing order of their priority until
 * one succeeded to branch. Since most branching rules are able to generate a branching in all situations,
 * only the rule of highest priority is used. In combination with the BRANCHRULE_MAXDEPTH and
 * BRANCHRULE_MAXBOUNDDIST settings, however, interesting strategies can be easily employed. For example,
 * the user can set the priority of the "full strong branching" strategy to the highest value and assign the
 * second highest value to the "reliable pseudo cost" rule. If (s)he also sets the maximal depth for the
 * "full strong branching" to 5, in the top 5 depth levels of the search tree the "full strong branching" is
 * applied, while in the deeper levels "reliable pseudo cost branching" is used.
 * \n
 * Note that the BRANCHRULE_PRIORITY property only specifies the default value of the priority. The user can
 * change this value arbitrarily.
 *
 * \par BRANCHRULE_MAXDEPTH: the default value for the maximal depth level of the branching rule.
 * This parameter denotes the maximal depth level in the branch-and-bound tree up to which the branching method of the
 * branching rule will be applied. Use -1 for no limit.
 * \n
 * Note that this property only specifies the default value. The user can change this value arbitrarily.
 *
 * \par BRANCHRULE_MAXBOUNDDIST: the default value for the maximal relative distance from current node's dual bound to primal bound compared to best node's dual bound for applying branching.
 * At the current branch-and-bound node, the relative distance from its dual bound (local dual bound)
 * to the primal bound compared to the best node's dual bound (global dual bound) is considered. The branching method of
 * the branching rule will only be applied at the node if this relative distance does not exceed BRANCHRULE_MAXBOUNDDIST.
 * \n
 * For example, if the global dual bound is 50 and the primal bound is 60, BRANCHRULE_MAXBOUNDDIST = 0.25 means that
 * branching is only applied if the current node's dual bound is in the first quarter of the interval [50,60], i.e., if it
 * is less than or equal to 52.5. In particular, the values 0.0 and 1.0 mean that the branching rule is applied at the
 * current best node only or at all nodes, respectively.
 * \n
 * Note that the BRANCHRULE_MAXBOUNDDIST property only specifies the default value of the maximal bound distance.
 * The user can change this value arbitrarily.
 *
 *
 * @section BRANCHRULE_DATA Branching Rule Data
 *
 * Below the header "Data structures" you can find a struct which is called "struct SCIP_BranchruleData".
 * In this data structure, you can store the data of your branching rule. For example, you should store the adjustable
 * parameters of the branching rule in this data structure.
 * If you are using C++, you can add branching rule data as usual as object variables to your class.
 * \n
 * Defining branching rule data is optional. You can leave the struct empty.
 *
 *
 * @section BRANCHRULE_INTERFACE Interface Methods
 *
 * At the bottom of "branch_mybranchingrule.c", you can find the interface method SCIPincludeBranchruleMybranchingrule(),
 * which also appears in "branch_mybranchingrule.h"
 * SCIPincludeBranchruleMybranchingrule() is called by the user, if (s)he wants to include the branching rule,
 * i.e., if (s)he wants to use the branching rule in his/her application.
 *
 * This method only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the branching rule. For this, you can either call
 * SCIPincludeBranchrule(),
 * or SCIPincludeBranchruleBasic() since SCIP version 3.0. In the latter variant, \ref BRANCHRULE_ADDITIONALCALLBACKS "additional callbacks"
 * must be added via setter functions as, e.g., SCIPsetBranchruleCopy(). We recommend this latter variant because
 * it is more stable towards future SCIP versions which might have more callbacks, whereas source code using the first
 * variant must be manually adjusted with every SCIP release containing new callbacks for branchrule in order to compile.
 *
 *
 * If you are using branching rule data, you have to allocate the memory for the data at this point.
 * You can do this by calling:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &branchruledata) );
 * \endcode
 * You also have to initialize the fields in struct SCIP_BranchruleData afterwards.
 *
 * You may also add user parameters for your branching rule, see the method SCIPincludeBranchruleRelpscost() in
 * src/scip/branch_relpscost.c for an example.
 *
 *
 * @section BRANCHRULE_FUNDAMENTALCALLBACKS Fundamental Callback Methods of a Branching Rule
 *
 * Branching rules do not have any fundamental callback methods, i.e., all callback methods are optional.
 * In most cases, however, you want to implement the \ref BRANCHEXECLP method and sometimes the \ref BRANCHEXECPS method.
 *
 *
 * @section BRANCHRULE_ADDITIONALCALLBACKS Additional Callback Methods of a Branching Rule
 *
 * The additional callback methods do not need to be implemented in every case. However, some of them have to be
 * implemented for most applications, they can be used, for example, to initialize and free private data.
 * Additional callbacks can either be passed directly with SCIPincludeBranchrule() to SCIP or via specific
 * <b>setter functions</b> after a call of SCIPincludeBranchruleBasic(), see also @ref BRANCHRULE_INTERFACE.
 *
 * The most important callback methods are the \ref BRANCHEXECLP, \ref BRANCHEXECEXT,
 * and \ref BRANCHEXECPS methods, which perform the actual task of generating a branching.
 *
 * Additional documentation for the callback methods can be found in type_branch.h.
 *
 * @subsection BRANCHEXECLP
 *
 * The BRANCHEXECLP callback is executed during node processing if a fractional LP solution is available. It should
 * split the current problem into smaller subproblems. Usually, the branching is done in a way such that the current
 * fractional LP solution is no longer feasible in the relaxation of the subproblems.  It is, however, possible to
 * create a child node for which the fractional LP solution is still feasible in the relaxation, for example, by
 * branching on a variable with integral LP value.  In every case, you have to make sure that each subproblem is a
 * proper restriction of the current problem.  Otherwise, you risk to produce an infinite path in the search tree.
 *
 * The user gains access to the branching candidates, i.e., to the fractional variables, and their LP solution values by
 * calling the method SCIPgetLPBranchCands(). Furthermore, SCIP provides two methods for performing the actual
 * branching, namely SCIPbranchVar() and SCIPcreateChild().
 *
 * Given an integral variable \f$x\f$ with fractional LP solution value \f$x^*\f$, the method SCIPbranchVar() creates
 * two child nodes; one contains the bound \f$x \le \lfloor x^* \rfloor\f$ and the other one contains the bound \f$x \ge
 * \lceil x^* \rceil\f$, see the BRANCHEXECLP callback in src/scip/branch_mostinf.c for an example. In addition, if a
 * proven lower objective bound of a created child node is known, like after strong branching has been applied, the user
 * may call the method SCIPupdateNodeLowerbound() in order to update the child node's lower bound.
 *
 * Please also see the \ref BRANCHEXEC "further information for the three execution methods".
 *
 * @subsection BRANCHEXECEXT
 *
 * The BRANCHEXECEXT callback is executed during node processing if no LP solution is available and the list of
 * external branching candidates is not empty. It should split the current problem into smaller subproblems. If you
 * do not use relaxation handlers or constraints handlers that provide external branching candidates, you do not need to
 * implement this callback.
 *
 * In contrast to the LP branching candidates and the pseudo branching candidates, the list of external branching
 * candidates will not be generated automatically. The user has to add all variables to the list by calling
 * SCIPaddExternBranchCand() for each of them. Usually, this will happen in the execution method of a relaxation handler or in the
 * enforcement methods of a constraint handler.
 *
 * The user gains access to these branching candidates by calling the method SCIPgetExternBranchCands(). Furthermore,
 * SCIP provides two methods for performing the actual branching with a given solution value, namely SCIPbranchVarVal()
 * and SCIPcreateChild(). SCIPbranchVarVal() allows users to specify the branching point for a variable in contrast to
 * SCIPbranchVar(), which will always use the current LP or pseudo solution.
 *
 * This paragraph contains additional information regarding how the method SCIPbranchVarVal() works. For external branching candidates,
 * there are three principle possibilities:
 * - Given a continuous variable \f$x\f$ with solution value \f$x^*\f$, the method SCIPbranchVarVal() creates
 *   two child nodes; one contains the bound \f$x \le x^* \f$ and the other one contains the bound \f$x \ge x^* \f$.
 * - Given an integer variable \f$x\f$ with fractional solution value \f$x^*\f$, the method
 *   SCIPbranchVarVal() creates two child nodes; one contains the bound \f$x \le \lfloor x^* \rfloor\f$ and the other
 *   one contains the bound \f$x \ge \lceil x^* \rceil\f$.
 * - Given an integer variable \f$x\f$ with integral solution value \f$x^*\f$, the method SCIPbranchVarVal()
 *   creates three child nodes; one contains the bound \f$x \le x^* -1\f$, one contains the bound \f$x \ge x^* +1\f$,
 *   one contains the fixing \f$x = x^*\f$.
 *
 * See the BRANCHEXECEXT callback in src/scip/branch_random.c for an example. In addition, if a proven lower bound of a
 * created child node is known the user may call the method SCIPupdateNodeLowerbound() in order to update the child
 * node's lower bound.
 *
 * Please also see the \ref BRANCHEXEC "further information for the three execution methods".
 *
 * @subsection BRANCHEXECPS
 *
 * The BRANCHEXECPS callback is executed during node processing if no LP solution is available and at least one of the
 * integer variables is not yet fixed. It should split the current problem into smaller subproblems. PS stands for
 * pseudo solution which is the vector of all variables set to their locally best (w.r.t. the objective function)
 * bounds.
 *
 * The user gains access to the branching candidates, i.e., to the non-fixed integer variables, by calling the method
 * SCIPgetPseudoBranchCands(). Furthermore, SCIP provides two methods for performing the actual branching, namely
 * SCIPbranchVar() and SCIPcreateChild().
 *
 * Given an integer variable \f$x\f$ with bounds \f$[l,u]\f$ and not having solved the LP, the method SCIPbranchVar()
 * creates two child nodes:
 * - If both bounds are finite, then the two children will contain the domain reductions \f$x \le x^*\f$, and \f$x \ge
 *   x^*+1\f$ with \f$x^* = \lfloor \frac{l + u}{2}\rfloor\f$. The current pseudo solution will remain feasible in one
 *   of the branches, but the hope is that halving the domain's size leads to good propagations.
 * - If only one of the bounds is finite, the variable will be fixed to that bound in one of the child nodes. In the
 *   other child node, the bound will be shifted by one.
 * - If both bounds are infinite, three children will be created: \f$x \le 1\f$, \f$x \ge 1\f$, and \f$x = 0\f$.

 *
 * See the BRANCHEXECPS callback in src/scip/branch_random.c for an example. In addition, if a proven lower bound of a
 * created child node is known, the user may call the method SCIPupdateNodeLowerbound() in order to update the child
 * node's lower bound.
 *
 * Please also see the \ref BRANCHEXEC "further information for the three execution methods".
 *
 * @subsection BRANCHEXEC Further information for the three execution methods
 *
 * In order to apply more general branching schemes, one should use the method SCIPcreateChild().
 * After having created a child node, the additional restrictions of the child node have to be added with calls to
 * SCIPaddConsNode(), SCIPchgVarLbNode(), or SCIPchgVarUbNode().
 * \n
 * In the method SCIPcreateChild(), the branching rule has to assign two values to the new nodes: a node selection
 * priority for each node and an estimate for the objective value of the best feasible solution contained in the subtree
 * after applying the branching. If the method SCIPbranchVar() is used, these values are automatically assigned. For
 * variable based branching schemes, one might use the methods SCIPcalcNodeselPriority() and the method
 * SCIPcalcChildEstimate().
 *
 * In some cases, the branching rule can tighten the current subproblem instead of producing a branching. For example,
 * strong branching might have proven that rounding up a variable would lead to an infeasible LP relaxation and thus,
 * the variable must be rounded down. Therefore, the BRANCHEXECLP, BRANCHEXECPS and BRANCHEXECREL callbacks may also
 * produce domain reductions or add additional constraints to the current subproblem.
 *
 * The execution callbacks have the following options:
 *  - detecting that the node is infeasible and can be cut off (result SCIP_CUTOFF)
 *  - adding an additional constraint (e.g. a conflict constraint) (result SCIP_CONSADDED; note that this action
 *    must not be performed if the input "allowaddcons" is FALSE)
 *  - reducing the domain of a variable such that the current LP solution becomes infeasible (result SCIP_REDUCEDDOM)
 *  - applying a branching (result SCIP_BRANCHED)
 *  - stating that the branching rule was skipped (result SCIP_DIDNOTRUN).
 *
 * Only the BRANCHEXECLP callback has the possibility to add a cutting plane to the LP (result SCIP_SEPARATED).
 *
 * @subsection BRANCHFREE
 *
 * If you are using branching rule data, you have to implement this method in order to free the branching rule data.
 * This can be done by the following procedure:
 *
 * @refsnippet{src/scip/branch_random.c,SnippetBranchFreeRandom}
 *
 * If you have allocated memory for fields in your branching rule data, remember to free this memory
 * before freeing the branching rule data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 * @subsection BRANCHINIT
 *
 * The BRANCHINIT callback is executed after the problem is transformed.
 * The branching rule may, e.g., use this call to initialize its branching rule data.
 *
 * @subsection BRANCHCOPY
 *
 * The BRANCHCOPY callback is executed when a SCIP instance is copied, e.g. to
 * solve a sub-SCIP. By
 * defining this callback as
 * <code>NULL</code> the user disables the execution of the specified
 * branching rule for all copied SCIP instances. This may deteriorate the performance
 * of primal heuristics using sub-SCIPs.
 *
 * @subsection BRANCHEXIT
 *
 * The BRANCHEXIT callback is executed before the transformed problem is freed.
 * In this method, the branching rule should free all resources that have been allocated for the solving process in
 * BRANCHINIT.
 *
 * @subsection BRANCHINITSOL
 *
 * The BRANCHINITSOL callback is executed when the presolving is finished and the branch-and-bound process is about to
 * begin.
 * The branching rule may use this call to initialize its branch-and-bound specific data.
 *
 * @subsection BRANCHEXITSOL
 *
 * The BRANCHEXITSOL callback is executed before the branch-and-bound process is freed.
 * The branching rule should use this call to clean up its branch-and-bound data.
 */


/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page CUTSEL How to add cut selectors
 *
 * Cut selectors are used to select the cuts that are going to be added to the relaxation.
 * For more information on how SCIP manages cuts, see "What is the difference between a sepastore and the cutpool?" in the
 * \ref FAQ.
 * \n
 * A complete list of all cut selectors contained in this release can be found \ref CUTSELECTORS "here".
 *
 * We now explain how users can add their own cut selectors.
 * Take the hybrid cut selector (src/scip/cutsel_hybrid.c) as an example.
 * As all other default plugins, it is written in C. C++ users can easily adapt the code by using the scip::ObjCutsel wrapper
 * base class and implement the scip_...() virtual methods instead of the SCIP_DECL_CUTSEL... callback methods.
 *
 * Additional documentation for the callback methods of a cut selector can be found in the file type_cutsel.h.
 *
 * Here is what you have to do to implement a cut selector:
 * -# Copy the template files src/scip/cutsel_xyz.c and src/scip/cutsel_xyz.h into files named "cutsel_mycutselector.c"
 *    and "cutsel_mycutselector.h".
 *    \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPincludeCutselMycutselector() in oder to include the cut selector into your SCIP instance,
 *    e.g., in the main file of your project (see, e.g., src/cmain.c in the Binpacking example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "mycutselector".
 * -# Adjust the properties of the cut selector (see \ref CUTSEL_PROPERTIES).
 * -# Define the cut selector data (see \ref CUTSEL_DATA). This is optional.
 * -# Implement the interface methods (see \ref CUTSEL_INTERFACE).
 * -# Implement the fundamental callback methods (see \ref CUTSEL_FUNDAMENTALCALLBACKS).
 * -# Implement the additional callback methods (see \ref CUTSEL_ADDITIONALCALLBACKS). This is optional.
 *
 *
 * @section CUTSEL_PROPERTIES Properties of a Cut Selector
 *
 * At the top of the new file "cutsel_mycutselector.c" you can find the cut selector properties.
 * These are given as compiler defines.
 * In the C++ wrapper class, you have to provide the cut selector properties by calling the constructor
 * of the abstract base class scip::ObjCutsel from within your constructor.
 * The properties you have to set have the following meaning:
 *
 * \par CUTSEL_NAME: the name of the cut selector.
 * This name is used in the interactive shell to address the cut selector.
 * Additionally, if you are searching for a cut selector with SCIPfindCutsel(), this name is looked up.
 * Names have to be unique: no two cut selectors may have the same name.
 *
 * \par CUTSEL_DESC: the description of the cut selector.
 * This string is printed as a description of the cut selector in the interactive shell.
 *
 * \par CUTSEL_PRIORITY: the priority of the cut selector.
 * After all cuts have been collected in the sepastore, SCIP asks the cut selectors to select cuts.
 * The cut selectors are sorted by priority (highest goes first) and are called, in this order, until the first one
 * succeeds.
 * \n
 * Note that this property only defines the default value of the priority. The user may change this value arbitrarily by
 * adjusting the corresponding parameter setting. Whenever, even during solving, the priority of a cut selector is
 * changed, the cut selectors are resorted by the new priorities.
 *
 *
 * @section CUTSEL_DATA Cut Selector Data
 *
 * Below the header "Data structures" you can find a struct which is called "struct SCIP_CutselData".
 * In this data structure, you can store the data of your cut selector. For example, you should store the adjustable
 * parameters of the cut selector in this data structure.
 * If you are using C++, you can add cut selector data as usual as object variables to your class.
 * \n
 * Defining cut selector data is optional. You can leave the struct empty.
 *
 *
 * @section CUTSEL_INTERFACE Interface Methods
 *
 * At the bottom of "cutsel_mycutselector.c", you can find the interface method SCIPincludeCutselMycutselector(),
 * which also appears in "cutsel_mycutselector.h"
 * SCIPincludeCutselMycutselector() is called by the user, if they want to include the cut selector, i.e., if they want
 * to use the cut selector in their application.
 *
 * This method only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the cut selector. For this, you can either call
 * SCIPincludeCutsel()
 * or SCIPincludeCutselBasic(). In the latter variant, \ref CUTSEL_ADDITIONALCALLBACKS "additional callbacks"
 * must be added via setter functions as, e.g., SCIPsetCutselCopy(). We recommend this latter variant because
 * it is more stable towards future SCIP versions which might have more callbacks, whereas source code using the first
 * variant must be manually adjusted with every SCIP release containing new callbacks for cut selectors in order to compile.
 *
 *
 * If you are using cut selector data, you have to allocate the memory for the data at this point.
 * You can do this by calling:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &cutseldata) );
 * \endcode
 * You also have to initialize the fields in struct SCIP_CutselData afterwards.
 *
 * You may also add user parameters for your cut selector, see the method SCIPincludeCutselHybrid() in
 * src/scip/cutsel_hybrid.c for an example.
 *
 *
 * @section CUTSEL_FUNDAMENTALCALLBACKS Fundamental Callback Methods of a Cut Selector
 *
 * The fundamental callback methods of the plugins are the ones that have to be implemented in order to obtain
 * an operational algorithm.
 * They are passed together with the cut selector itself to SCIP using SCIPincludeCutsel() or SCIPincludeCutselBasic(),
 * see @ref CUTSEL_INTERFACE.
 *
 * Cut selector plugins have a single fundamental callback method, namely the CUTSELSELECT method.
 * This method has to be implemented for every cut selector; the other callback methods are optional.
 * It implements the single contribution every selector has to provide: Selecting cuts to be added to the relaxation.
 * In the C++ wrapper class scip::ObjCutsel, the scip_select() method is a virtual abstract member function.
 * You have to implement it in order to be able to construct an object of your cut selector class.
 *
 * @subsection CUTSELSELECT
 *
 * The CUTSELSELECT callback should decide which cuts should be added to the relaxation.
 * The callback receives the arrays of cuts to select from. This array must be resorted and the first nselectedcuts from
 * the sorted array are going to be selected.
 * In addition to the aforementioned cuts, the list of forced cuts is also given as an argument. This array can be used
 * to help with the selection algorithm. Note, however, that this array should not be tampered with.
 *
 * Additional documentation for this callback can be found in type_cutsel.h.
 *
 * @section CUTSEL_ADDITIONALCALLBACKS Additional Callback Methods of a Cut Selector
 *
 * The additional callback methods do not need to be implemented in every case. However, some of them have to be
 * implemented for most applications. They can be used, for example, to initialize and free private data.
 * Additional callbacks can either be passed directly with SCIPincludeCutsel() to SCIP or via specific
 * <b>setter functions</b> after a call of SCIPincludeCutselBasic(), see also @ref CUTSEL_INTERFACE.
 *
 * @subsection CUTSELFREE
 *
 * If you are using cut selector data, you have to implement this method in order to free the cut selector data.
 * This can be done by the following procedure:
 *
 * @refsnippet{src/scip/cutsel_hybrid.c,SnippetCutselFreeHybrid}
 *
 * If you have allocated memory for fields in your cut selector data, remember to free this memory
 * before freeing the cut selector data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 * @subsection CUTSELINIT
 *
 * The CUTSELINIT callback is executed after the problem is transformed.
 * The cut selector may, for example, use this call to initialize its cut selector data.
 *
 * @subsection CUTSELCOPY
 *
 * The CUTSELCOPY callback is executed when a SCIP instance is copied, e.g., to solve a sub-SCIP. By defining this
 * callback as <code>NULL</code> the user disables the execution of the specified cut selector for all copied SCIP
 * instances.
 *
 * @subsection CUTSELEXIT
 *
 * The CUTSELEXIT callback is executed before the transformed problem is freed.
 * In this method, the cut selector should free all resources that have been allocated for the solving process
 * in CUTSELINIT.
 *
 * @subsection CUTSELINITSOL
 *
 * The CUTSELINITSOL callback is executed when the presolving is finished and the branch-and-bound process is about to
 * begin.
 * The cut selector may use this call to initialize its branch-and-bound specific data.
 *
 * @subsection CUTSELEXITSOL
 *
 * The CUTSELEXITSOL callback is executed before the branch-and-bound process is freed.
 * The cut selector should use this call to clean up its branch-and-bound data.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page NODESEL How to add node selectors
 *
 * Node selectors are used to decide which of the leaves in the current branching tree is selected as next subproblem
 * to be processed. The ordering relation of the tree's leaves for storing them in the leaf priority queue is also
 * defined by the node selectors.
 * \n
 * A complete list of all node selectors contained in this release can be found \ref NODESELECTORS "here".
 *
 * We now explain how users can add their own node selectors.
 * Take the node selector for depth first search (src/scip/nodesel_dfs.c) as an example.
 * As all other default plugins, it is written in C. C++ users can easily adapt the code by using the scip::ObjNodesel wrapper
 * base class and implement the scip_...() virtual methods instead of the SCIP_DECL_NODESEL... callback methods.
 *
 * Additional documentation for the callback methods of a node selector can be found in the file type_nodesel.h.
 *
 * Here is what you have to do to implement a node selector:
 * -# Copy the template files src/scip/nodesel_xyz.c and src/scip/nodesel_xyz.h into files named "nodesel_mynodeselector.c"
 *    and "nodesel_mynodeselector.h".
 *    \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPincludeNodeselMynodeselector() in oder to include the node selector into your SCIP instance,
 *    e.g., in the main file of your project (see, e.g., src/cmain.c in the Binpacking example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "mynodeselector".
 * -# Adjust the properties of the node selector (see \ref NODESEL_PROPERTIES).
 * -# Define the node selector data (see \ref NODESEL_DATA). This is optional.
 * -# Implement the interface methods (see \ref NODESEL_INTERFACE).
 * -# Implement the fundamental callback methods (see \ref NODESEL_FUNDAMENTALCALLBACKS).
 * -# Implement the additional callback methods (see \ref NODESEL_ADDITIONALCALLBACKS). This is optional.
 *
 *
 * @section NODESEL_PROPERTIES Properties of a Node Selector
 *
 * At the top of the new file "nodesel_mynodeselector.c" you can find the node selector properties.
 * These are given as compiler defines.
 * In the C++ wrapper class, you have to provide the node selector properties by calling the constructor
 * of the abstract base class scip::ObjNodesel from within your constructor.
 * The properties you have to set have the following meaning:
 *
 * \par NODESEL_NAME: the name of the node selector.
 * This name is used in the interactive shell to address the node selector.
 * Additionally, if you are searching for a node selector with SCIPfindNodesel(), this name is looked up.
 * Names have to be unique: no two node selectors may have the same name.
 *
 * \par NODESEL_DESC: the description of the node selector.
 * This string is printed as a description of the node selector in the interactive shell.
 *
 * \par NODESEL_STDPRIORITY: the default priority of the node selector in the standard mode.
 * The first step of each iteration of the main solving loop is the selection of the next subproblem to be processed.
 * The node selector of highest priority (the active node selector) is called to do this selection.
 * In particular, if you implemented your own node selector plugin which you want to be applied, you should choose a priority
 * which is greater then all priorities of the SCIP default node selectors.
 * Note that SCIP has two different operation modes: the standard mode and the memory saving mode. If the memory
 * limit - given as a parameter by the user - is almost reached, SCIP switches from the standard mode to the memory saving
 * mode in which different priorities for the node selectors are applied. NODESEL_STDPRIORITY is the priority of the
 * node selector used in the standard mode.
 * \n
 * Note that this property only defines the default value of the priority. The user may change this value arbitrarily by
 * adjusting the corresponding parameter setting.
 *
 * \par NODESEL_MEMSAVEPRIORITY: the default priority of the node selector in the memory saving mode.
 * The priority NODESEL_MEMSAVEPRIORITY of the node selector has the same meaning as the priority NODESEL_STDPRIORITY, but
 * is used in the memory saving mode.
 * Usually, you want the best performing node selector, for example best estimate search, to have maximal
 * standard priority, while you want a node selector which tends to keep the growth of the search tree limited, for example
 * depth first search, to have maximal memory saving priority.
 * \n
 * Note that this property only defines the default value of the priority. The user may change this value arbitrarily by
 * adjusting the corresponding parameter setting.
 *
 *
 * @section NODESEL_DATA Node Selector Data
 *
 * Below the header "Data structures" you can find a struct which is called "struct SCIP_NodeselData".
 * In this data structure, you can store the data of your node selector. For example, you should store the adjustable
 * parameters of the node selector in this data structure.
 * If you are using C++, you can add node selector data as usual as object variables to your class.
 * \n
 * Defining node selector data is optional. You can leave the struct empty.
 *
 *
 * @section NODESEL_INTERFACE Interface Methods
 *
 * At the bottom of "nodesel_mynodeselector.c", you can find the interface method SCIPincludeNodeselMynodeselector(),
 * which also appears in "nodesel_mynodeselector.h"
 * SCIPincludeNodeselMynodeselector() is called by the user, if (s)he wants to include the node selector,
 * i.e., if (s)he wants to use the node selector in his/her application.
 *
 * This method only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the node selector. For this, you can either call
 * SCIPincludeNodesel(),
 * or SCIPincludeNodeselBasic() since SCIP version 3.0. In the latter variant, \ref NODESEL_ADDITIONALCALLBACKS "additional callbacks"
 * must be added via setter functions as, e.g., SCIPsetNodeselCopy(). We recommend this latter variant because
 * it is more stable towards future SCIP versions which might have more callbacks, whereas source code using the first
 * variant must be manually adjusted with every SCIP release containing new callbacks for node selectors in order to compile.
 *
 *
 * If you are using node selector data, you have to allocate the memory for the data at this point.
 * You can do this by calling:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &nodeseldata) );
 * \endcode
 * You also have to initialize the fields in struct SCIP_NodeselData afterwards.
 *
 * You may also add user parameters for your node selector, see the method SCIPincludeNodeselRestartdfs() in
 * src/scip/nodesel_restartdfs.c for an example.
 *
 *
 * @section NODESEL_FUNDAMENTALCALLBACKS Fundamental Callback Methods of a Node Selector
 *
 * The fundamental callback methods of the plugins are the ones that have to be implemented in order to obtain
 * an operational algorithm.
 * They are passed together with the node selector itself to SCIP using SCIPincludeNodesel() or SCIPincludeNodeselBasic(),
 * see @ref NODESEL_INTERFACE.
 *
 * Node selector plugins have two fundamental callback methods, namely the NODESELSELECT method and the NODESELCOMP method.
 * These methods have to be implemented for every node selector; the other callback methods are optional.
 * They implement the two requirements every node selector has to fulfill: Selecting a node from the queue to be processed
 * next and, given two nodes, deciding which of both is favored by the node selector's selection rule. The first
 * task is implemented in the NODESELSELECT callback, the second one in the NODESELCOMP callback.
 * In the C++ wrapper class scip::ObjNodesel, the scip_select() method and the scip_comp() method (which correspond to the
 * NODESELSELECT callback and the NODESELCOMP callback, respectively) are virtual abstract member functions.
 * You have to implement them in order to be able to construct an object of your node selector class.
 *
 * Additional documentation for the callback methods can be found in type_nodesel.h.
 *
 * @subsection NODESELSELECT
 *
 * The NODESELSELECT callback is the first method called in each iteration in the main solving loop. It should decide
 * which of the leaves in the current branching tree is selected as the next subproblem to be processed.
 * It can arbitrarily decide between all leaves stored in the tree, but for performance reasons,
 * the current node's children and siblings are often treated different from the remaining leaves.
 * This is mainly due to the warm start capabilities of the simplex algorithm and the expectation that the bases of
 * neighboring vertices in the branching tree very similar.
 * The node selector's choice of the next node to process can
 * have a large impact on the solver's performance, because it influences the finding of feasible solutions and the
 * development of the global dual bound.
 *
 * Besides the ranking of the node selector, every node gets assigned a node selection priority by the branching rule
 * that created the node. See the \ref BRANCHEXECLP and \ref BRANCHEXECPS callbacks of the branching rules for details.
 * For example, the node where the branching went in the same way as the deviation from the branching variable's
 * root solution could be assigned a higher priority than the node where the branching went in the opposite direction.
 *
 * The following methods provide access to the various types of leaf nodes:
 * - SCIPgetPrioChild() returns the child of the current node with the largest node selection priority, as assigned by the
 *   branching rule.
 *   If no child is available (for example, because the current node was pruned), a NULL pointer is returned.
 * - SCIPgetBestChild() returns the best child of the current node with respect to the node selector's ordering relation as
 *   defined by the \ref NODESELCOMP callback. If no child is available, a NULL pointer is returned.
 * - SCIPgetPrioSibling() returns the sibling of the current node with the largest node selection priority.
 *   If no sibling is available (for example, because all siblings of the current node have already been processed), a NULL
 *   pointer is returned.
 *   Note that in binary branching every node has at most one sibling, but since SCIP supports arbitrary branching rules,
 *   this might not always be the case.
 * - SCIPgetBestSibling() returns the best sibling of the current node with respect to the node selector's ordering relation
 *   as defined by the \ref NODESELCOMP callback. If no sibling is available, a NULL pointer is returned.
 * - SCIPgetBestNode() returns the best leaf from the tree (children, siblings, or other leaves) with respect to the node
 *   selector's ordering relation as defined by the \ref NODESELCOMP callback. If no open leaf exists, a NULL pointer is
 *   returned. In this case, the optimization is finished, and the node selector should return a NULL pointer as 'selnode'.
 * - SCIPgetBestboundNode() returns a leaf from the tree (children, siblings, or other leaves) with the smallest lower (dual)
 *   objective bound. If the queue is empty, a NULL pointer is returned. In this case, the optimization is finished, and the
 *   node selector should return a NULL pointer as 'selnode'.
 *
 *
 * @subsection NODESELCOMP
 *
 * The NODESELCOMP callback is called to compare two leaves of the current branching tree (say node 1 and node 2)
 * regarding their ordering relation.
 *
 * The NODESELCOMP should return the following values:
 *  - value < 0, if node 1 comes before (is better than) node 2
 *  - value = 0, if both nodes are equally good
 *  - value > 0, if node 1 comes after (is worse than) node 2.
 *
 * @section NODESEL_ADDITIONALCALLBACKS Additional Callback Methods of a Node Selector
 *
 * The additional callback methods do not need to be implemented in every case. However, some of them have to be
 * implemented for most applications, they can be used, for example, to initialize and free private data.
 * Additional callbacks can either be passed directly with SCIPincludeNodesel() to SCIP or via specific
 * <b>setter functions</b> after a call of SCIPincludeNodeselBasic(), see also @ref NODESEL_INTERFACE.
 *
 * @subsection NODESELFREE
 *
 * If you are using node selector data, you have to implement this method in order to free the node selector data.
 * This can be done by the following procedure:
 *
 * @refsnippet{src/scip/nodesel_bfs.c,SnippetNodeselFreeBfs}
 *
 * If you have allocated memory for fields in your node selector data, remember to free this memory
 * before freeing the node selector data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 * @subsection NODESELINIT
 *
 * The NODESELINIT callback is executed after the problem is transformed.
 * The node selector may, e.g., use this call to initialize its node selector data.
 *
 * @subsection NODESELCOPY
 *
 * The NODESELCOPY callback is executed when a SCIP instance is copied, e.g. to
 * solve a sub-SCIP. By
 * defining this callback as
 * <code>NULL</code> the user disables the execution of the specified
 * node selector for all copied SCIP instances. This may deteriorate the performance
 * of primal heuristics using sub-SCIPs.
 *
 * @subsection NODESELEXIT
 *
 * The NODESELEXIT callback is executed before the transformed problem is freed.
 * In this method, the node selector should free all resources that have been allocated for the solving process
 * in NODESELINIT.
 *
 * @subsection NODESELINITSOL
 *
 * The NODESELINITSOL callback is executed when the presolving is finished and the branch-and-bound process is about to
 * begin.
 * The node selector may use this call to initialize its branch-and-bound specific data.
 *
 * @subsection NODESELEXITSOL
 *
 * The NODESELEXITSOL callback is executed before the branch-and-bound process is freed.
 * The node selector should use this call to clean up its branch-and-bound data.
 */


/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page HEUR How to add primal heuristics
 *
 * Feasible solutions can be found in two different ways during the traversal of the branch-and-bound tree. On one
 * hand, the solution of a node's relaxation may be feasible with respect to the constraints (including the integrality).
 * On the other hand, feasible solutions can be discovered by primal heuristics.
 * \n
 * A complete list of all primal heuristics contained in this release can be found \ref PRIMALHEURISTICS "here".
 * \n
 * Diving heuristics are primal heuristics that explore an auxiliary search tree in a depth-first manner. Since SCIP
 * version 3.2, it is easy to integrate further diving heuristics by using a special controller for the scoring,
 * see \ref DIVINGHEUR "here" for information on how to implement a diving heuristic.
 * \n
 * We now explain how users can add their own primal heuristics.
 * Take the simple and fast LP rounding heuristic (src/scip/heur_simplerounding.c) as an example.
 * The idea of simple rounding is to iterate over all fractional variables of an LP solution and round them down,
 * if the variables appears only with nonnegative coefficients in the system Ax <= b and round them up if
 * the variables appears only with nonpositive coefficients.
 * If one of both conditions applies for each of the fractional variables, this will give a feasible solution.
 * As all other default plugins, it is written in C. C++ users can easily adapt the code by using the scip::ObjHeur wrapper
 * base class and implement the scip_...() virtual methods instead of the SCIP_DECL_HEUR... callback methods.
 *
 * Additional documentation for the callback methods of a primal heuristic can be found in the file type_heur.h.
 *
 * Here is what you have to do to implement a primal heuristic:
 * -# Copy the template files src/scip/heur_xyz.c and src/scip/heur_xyz.h into files named "heur_myheuristic.c"
 *    and "heur_myheuristic.h".
 *    \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPincludeHeurMyheuristic() in order to include the heuristic into your SCIP instance,
 *    e.g., in the main file of your project (see, e.g., src/cmain.c in the Binpacking example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "myheuristic".
 * -# Adjust the properties of the primal heuristic (see \ref HEUR_PROPERTIES).
 * -# Define the primal heuristic data (see \ref HEUR_DATA). This is optional.
 * -# Implement the interface methods (see \ref HEUR_INTERFACE).
 * -# Implement the fundamental callback methods (see \ref HEUR_FUNDAMENTALCALLBACKS).
 * -# Implement the additional callback methods (see \ref HEUR_ADDITIONALCALLBACKS). This is optional.
 *
 *
 * @section HEUR_PROPERTIES Properties of a Primal Heuristic
 *
 * At the top of the new file "heur_myheuristic.c" you can find the primal heuristic properties.
 * These are given as compiler defines.
 * In the C++ wrapper class, you have to provide the primal heuristic properties by calling the constructor
 * of the abstract base class scip::ObjHeur from within your constructor.
 * Of course, all of them are of relevant, but the most important ones for controlling the performance
 * are usually HEUR_FREQ and HEUR_TIMING.
 * The properties you have to set have the following meaning:
 *
 * \par HEUR_NAME: the name of the primal heuristic.
 * This name is used in the interactive shell to address the primal heuristic.
 * Additionally, if you are searching for a primal heuristic with SCIPfindHeur(), this name is looked up.
 * Names have to be unique: no two primal heuristics may have the same name.
 *
 * \par HEUR_DESC: the description of the primal heuristic.
 * This string is printed as a description of the primal heuristic in the interactive shell when you call "display heuristics".
 *
 * \par HEUR_DISPCHAR: the display character of the primal heuristic.
 * In the interactive shell, this character is printed in the first column of a status information row, if the primal
 * heuristic found the feasible solution belonging to the primal bound. Note that a star '*' stands for an integral
 * LP-relaxation.
 * It is recommended to select a lower or upper case letter as display character. The default primal heuristics of
 * SCIP use characters that describe the class to which the heuristic belongs. As an example all LP rounding heuristics
 * have an 'r' and all Large Neighborhood Search heuristics use the letter 'L'.
 * Users find commonly used display characters in type_heur.h.
 *
 *
 * \par HEUR_PRIORITY: the priority of the primal heuristic.
 * At each of the different entry points of the primal heuristics during the solving process (see HEUR_TIMING), they are
 * called in decreasing order of their priority.
 * \n
 * The priority of a primal heuristic should be set according to the complexity of the heuristic and the likelihood to find
 * feasible solutions: primal heuristics that provide fast algorithms that often succeed in finding a feasible solution should have
 * a high priority (like simple rounding). In addition, the interaction between different types of primal heuristics should be taken into account.
 * For example, improvement heuristics, which try to generate improved solutions by inspecting one or more of the feasible
 * solutions that have already been found, should have a low priority (like Crossover which by default needs at least 3 feasible solutions).
 *
 * \par HEUR_FREQ: the default frequency for executing the primal heuristic.
 * The frequency together with the frequency offset (see HEUR_FREQOFS) defines the depth levels at which the execution
 * method of the primal heuristic \ref HEUREXEC is called. For example, a frequency of 7 together with a frequency offset
 * of 5 means, that the \ref HEUREXEC callback is executed for subproblems that are in depth 5, 12, 19, ... of the branching tree. A
 * frequency of 0 together with a frequency offset of 3 means, that the execution method is only called at those nodes that are in
 * depth level 3 (i.e., at most for \f$2^3 = 8\f$ nodes if binary branching is applied).
 * Typical cases are: A frequency of 0 and an offset of 0 which means that
 * the heuristic is only called at the root node and a frequency of -1 which disables the heuristic.
 * \n
 * The frequency can be adjusted by the user. This property of the primal heuristic only defines the default value of the
 * frequency. If you want to have a more flexible control of when to execute the primal heuristic, you have to assign
 * a frequency of 1 and implement a check at the beginning of your execution method whether you really want to search for feasible
 * solutions or not. If you do not want to execute the method, set the result code to SCIP_DIDNOTRUN.
 *
 * \par HEUR_FREQOFS: the frequency offset for executing the primal heuristic.
 * The frequency offset defines the depth of the branching tree at which the primal heuristic is executed for the first
 * time. For example, a frequency of 7 (see HEUR_FREQ) together with a frequency offset of 10 means, that the
 * callback is executed for subproblems that are in depth 10, 17, 24, ... of the branching tree. In particular, assigning
 * different offset values to heuristics of the same type, like diving heuristics, can be useful for evenly spreading the
 * application of these heuristics across the branch-and-bound tree.
 * Note that if the frequency is equal to 1, the heuristic is applied for all nodes with depth level larger or equal to
 * the frequency offset.
 *
 * \par HEUR_MAXDEPTH: the maximal depth level for executing the primal heuristic.
 * This parameter denotes the maximal depth level in the branching tree up to which the execution method of the primal
 * heuristic is called. Use -1 for no limit (a usual case).
 *
 * \par HEUR_TIMING: the execution timing of the primal heuristic.
 * Primal heuristics have different entry points during the solving process and the execution timing parameter defines the
 * entry point at which the primal heuristic is executed first.
 * \n
 * The primal heuristic can be called first:
 * - before the processing of the node starts (SCIP_HEURTIMING_BEFORENODE)
 * - after each LP solve during the cut-and-price loop (SCIP_HEURTIMING_DURINGLPLOOP)
 * - after the cut-and-price loop was finished (SCIP_HEURTIMING_AFTERLPLOOP)
 * - after the processing of a node <em>with solved LP</em>  was finished (SCIP_HEURTIMING_AFTERLPNODE)
 * - after the processing of a node <em>without solved LP</em> was finished (SCIP_HEURTIMING_AFTERPSEUDONODE)
 * - after the processing of the last node in the current plunge was finished, <em>and only if the LP was solved for
 *   this node</em> (SCIP_HEURTIMING_AFTERLPPLUNGE)
 * - after the processing of the last node in the current plunge was finished, <em>and only if the LP was not solved
 *   for this node</em> (SCIP_HEURTIMING_AFTERPSEUDOPLUNGE).
 * \par
 * A plunge is the successive solving of child and sibling nodes in the search tree.
 * The flags listed above can be combined to call the heuristic at multiple times by concatenating them with a bitwise OR.
 * Two useful combinations are already predefined:
 * - after the processing of a node was finished (SCIP_HEURTIMING_AFTERNODE; combines SCIP_HEURTIMING_AFTERLPNODE and
 *   SCIP_HEURTIMING_AFTERPSEUDONODE)
 * - after the processing of the last node in the current plunge was finished (SCIP_HEURTIMING_AFTERPLUNGE; combines
 *   SCIP_HEURTIMING_AFTERLPPLUNGE and SCIP_HEURTIMING_AFTERPSEUDOPLUNGE)
 * \par
 * Calling a primal heuristic "before the processing of the node starts" is particularly useful for heuristics
 * that do not need to access the LP solution of the current node. If such a heuristic finds a feasible solution, the
 * leaves of the branching tree exceeding the new primal bound are pruned. It may happen that even the current node can
 * be cut off without solving the LP relaxation. Combinatorial heuristics, like the farthest insert heuristic for the TSP
 * (see examples/TSP/src/HeurFarthestInsert.cpp), are often applicable at this point.
 * \n
 * Very fast primal heuristics that require an LP solution can also be called "after each LP solve during the
 * cut-and-price loop". Rounding heuristics, like the simple and fast LP rounding heuristic
 * (src/scip/heur_simplerounding.c), belong to this group of primal heuristics.
 * \n
 * Most heuristics, however, are called either after a node was completely processed
 * (e.g. expensive rounding heuristics like RENS), or even only after a full plunge was finished (e.g., diving heuristics).
 *
 * \par HEUR_USESSUBSCIP: Does the heuristic use a secondary SCIP instance?
 * Some heuristics and separators solve MIPs or SAT problems using a secondary SCIP instance. Examples are
 * Large Neighborhood Search heuristics such as RINS and Local Branching or the CGMIP separator. To avoid recursion,
 * these plugins usually deactivate all other plugins that solve MIPs. If a heuristic uses a secondary SCIP instance,
 * this parameter has to be TRUE and it is recommended to call SCIPsetSubscipsOff() for the secondary SCIP instance.
 *
 * Computational experiments indicate that for the overall performance of a MIP solver, it is important to evenly
 * spread the application of the heuristics across the branch-and-bound tree. Thus, the assignment of the parameters
 * HEUR_FREQ, HEUR_FREQOFS, and HEUR_TIMING should contribute to this aim.
 *
 * Note that all diving heuristics in the SCIP distribution (see, e.g., src/scip/heur_guideddiving.c) check whether other diving
 * heuristics have already been called at the current node. This can be done by comparing SCIPgetLastDivenode(scip) and
 * SCIPgetNNodes(scip). If the two are equal, and if the current node is not the root node (SCIPgetDepth(scip) > 0), diving
 * heuristics should be delayed by returning the result code 'SCIP_DELAYED'. This is an additional contribution to the goal of
 * not calling multiple similar heuristics at the same node.
 *
 *
 * @section HEUR_DATA Primal Heuristic Data
 *
 * Below the header "Data structures" you can find a struct which is called "struct SCIP_HeurData".
 * In this data structure, you can store the data of your primal heuristic. For example, you should store the adjustable
 * parameters of the primal heuristic or a working solution in this data structure.
 * If you are using C++, you can add primal heuristic data as usual as object variables to your class.
 * \n
 * Defining primal heuristic data is optional. You can leave the struct empty.
 *
 *
 * @section HEUR_INTERFACE Interface Methods
 *
 * At the bottom of "heur_myheuristic.c", you can find the interface method SCIPincludeHeurMyheuristic(),
 * which also appears in "heur_myheuristic.h"
 * SCIPincludeHeurMyheuristic() is called by the user, if (s)he wants to include the heuristic,
 * i.e., if (s)he wants to use the heuristic in his/her application.
 *
 * This method only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the heuristic. For this, you can either call
 * SCIPincludeHeur(),
 * or SCIPincludeHeurBasic() since SCIP version 3.0. In the latter variant, \ref HEUR_ADDITIONALCALLBACKS "additional callbacks"
 * must be added via setter functions as, e.g., SCIPsetHeurCopy(). We recommend this latter variant because
 * it is more stable towards future SCIP versions which might have more callbacks, whereas source code using the first
 * variant must be manually adjusted with every SCIP release containing new callbacks for heuristics in order to compile.
 *
 * If you are using primal heuristic data, you have to allocate the memory for the data at this point.
 * You can do this by calling:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &heurdata) );
 * \endcode
 * You also have to initialize the fields in struct SCIP_HeurData afterwards.
 *
 * You may also add user parameters for your primal heuristic, see the method SCIPincludeHeurFeaspump() in
 * src/scip/heur_oneopt.c for an example where a single Boolean parameter is added.
 *
 *
 * @section HEUR_FUNDAMENTALCALLBACKS Fundamental Callback Methods of a Primal Heuristic
 *
 * The fundamental callback methods of the plugins are the ones that have to be implemented in order to obtain
 * an operational algorithm.
 * They are passed together with the primal heuristic itself to SCIP using SCIPincludeHeur() or SCIPincludeHeurBasic(),
 * see @ref HEUR_INTERFACE.
 *
 *
 * Primal heuristic plugins have only one fundamental callback method, namely the HEUREXEC method.
 * This method has to be implemented for every primal heuristic; the other callback methods are optional.
 * In the C++ wrapper class scip::ObjHeur, the scip_exec() method (which corresponds to the HEUREXEC callback) is a virtual
 * abstract member function. You have to implement it in order to be able to construct an object of your primal heuristic
 * class.
 *
 * Additional documentation for the callback methods can be found in type_heur.h.
 *
 * @subsection HEUREXEC
 *
 * The HEUREXEC callback is called at different positions during the node processing loop, see HEUR_TIMING. It should
 * search for feasible solutions and add them to the solution pool. For creating a new feasible solution, the
 * methods SCIPcreateSol() and SCIPsetSolVal() can be used. Afterwards, the solution can be added to the storage by
 * calling the method SCIPtrySolFree() (or SCIPtrySol() and SCIPfreeSol()).
 *
 * The HEUREXEC callback gets a SCIP pointer, a pointer to the heuristic itself, the current point in the
 * solve loop and a result pointer as input (see type_heur.h).
 *
 * The heuristic has to set the result pointer appropriately!
 * Therefore it has the following options:
 *  - finding at least one feasible solution (result SCIP_FOUNDSOL)
 *  - stating that the primal heuristic searched, but did not find a feasible solution (result SCIP_DIDNOTFIND)
 *  - stating that the primal heuristic was skipped (result SCIP_DIDNOTRUN)
 *  - stating that the primal heuristic was skipped, but should be called again (result SCIP_DELAYED).
 *
 *
 * @section HEUR_ADDITIONALCALLBACKS Additional Callback Methods of a Primal Heuristic
 *
 * The additional callback methods do not need to be implemented in every case. However, some of them have to be
 * implemented for most applications, they can be used, for example, to initialize and free private data.
 * Additional callbacks can either be passed directly with SCIPincludeHeur() to SCIP or via specific
 * <b>setter functions</b> after a call of SCIPincludeHeurBasic(), see also @ref HEUR_INTERFACE.
 *
 * @subsection HEURFREE
 *
 * If you are using primal heuristic data, you have to implement this method in order to free the primal heuristic data.
 * This can be done by the following procedure:
 *
 * @refsnippet{applications/Coloring/src/heur_init.c,SnippetHeurFreeInit}
 *
 * If you have allocated memory for fields in your primal heuristic data, remember to free this memory
 * before freeing the primal heuristic data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 * @subsection HEURINIT
 *
 * The HEURINIT callback is executed after the problem is transformed.
 * The primal heuristic may, e.g., use this call to initialize its primal heuristic data.
 *
 * @subsection HEURCOPY
 *
 * The HEURCOPY callback is executed when a SCIP instance is copied, e.g. to
 * solve a sub-SCIP. By
 * defining this callback as
 * <code>NULL</code> the user disables the execution of the specified
 * heuristic for all copied SCIP instances. This may deteriorate the performance
 * of primal heuristics using sub-SCIPs.
 *
 * @subsection HEUREXIT
 *
 * The HEUREXIT callback is executed before the tDIVINGHEURransformed problem is freed.
 * In this method, the primal heuristic should free all resources that have been allocated for the solving process in
 * HEURINIT.
 *
 * @subsection HEURINITSOL
 *
 * The HEURINITSOL callback is executed when the presolving is finished and the branch-and-bound process is about to
 * begin. The primal heuristic may use this call to initialize its branch-and-bound specific data.
 *
 * @subsection HEUREXITSOL
 *
 * The HEUREXITSOL callback is executed before the branch-and-bound process is freed. The primal heuristic should use this
 * call to clean up its branch-and-bound data, which was allocated in HEURINITSOL.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page DIVINGHEUR How to implement a diving heuristic
 *
 * Diving heuristics are an important addon to the branch-and-cut search. A diving heuristic explores a single probing
 * path down the search tree. In contrast to the regular search guided by branching rule(s) and the selected
 * node selector, the diving is performed in an auxiliary tree originating from the focus node of the main
 * search tree where the heuristic was called. The advantage of this approach is that many different scoring mechanisms
 * can be safely tried as diving heuristic and may probably lead to better solutions. SCIP has a lot of diving heuristics
 * included in its default plugin set.
 * \n
 *
 * Since SCIP version 3.2, the diving heuristics have been redesigned to contain mainly the scoring function used by the
 * heuristic. In order to implement a user-defined diving heuristic, it is possible to create one (or several)
 * divesets that control the scoring mechanism and add them to the primal heuristic. This has the advantage that
 * less code is necessary to create a working diving heuristic. The SCIP statistics now also display some interesting statistics
 * about every diveset together in the section 'Diving Statistics'.
 * \n
 *
 * This page contains the necessary steps to understand and include a diveset into ones primal diving heuristic plugin. As
 * a prerequisite, you should understand the basic implementation steps for a primal heuristic, see \ref HEUR.
 * In order to make use of divesets, they must be included _after_ the primal heuristic to which they should belong
 * has been included, by using SCIPincludeDiveset(). This will create the data structure for the diveset and
 * append it to the list of divesets belonging to the heuristic, which can be retrieved later together with their number
 * by using SCIPheurGetDivesets() and SCIPheurGetNDivesets(), respectively. No further memory allocation or deletion is needed;
 * As a member of the heuristic, SCIP automatically takes care of freeing the diveset when it is exiting.
 * \n
 *
 * Before the inclusion, one may think of adjusting the various properties that a diveset offers to control
 * the behavior of the algorithm. These are subject to the following section.
 * \n
 *
 * It is mandatory to implement the fundamental scoring callback of the diveset, which is explained in more detail
 * in Section \ref DIVING_FUNDAMENTALCALLBACKS.
 * \n
 *
 * Once the properties have been carefully adjusted and the scoring
 * has been defined, use the method SCIPperformGenericDivingAlgorithm() inside the execution callback (\ref HEUREXEC) of the primal
 * heuristic to which the diveset belongs, after checking possible preliminaries that may not be met at all times of the search.
 * \n
 *
 * For a code example, we refer to \ref heur_guideddiving.h, which guides the diving into the direction of the current incumbent solution.
 * Before it calls SCIPperformGenericDivingAlgorithm(), it checks whether an incumbent is available, and returns if there is none.
 *
 *
 * @section DIVING_PARAMETERS User parameters and properties for every diveset
 *
 * Every diveset controls the diving behavior through a set of user-defined parameters, which are explained in the following:
 *
 * \par MINRELDEPTH
 * the minimal relative depth (to the maximum depth explored during regular search) of the current focus node to start diving
 *
 * \par MAXRELDEPTH
 * the maximal relative depth (to the maximum depth explored during regular search) of the current focus node to start diving
 *
 * \par MAXLPITERQUOT
 * maximal fraction of diving LP iterations compared to node LP iterations that this dive controller may consume
 *
 * \par MAXLPITEROFS
 * an additional number of allowed LP iterations
 *
 * \par MAXDIVEUBQUOT
 * maximal quotient (curlowerbound - lowerbound)/(cutoffbound - lowerbound)
 *   where diving is performed (0.0: no limit)
 *
 * \par MAXDIVEAVGQUOT
 * maximal quotient (curlowerbound - lowerbound)/(avglowerbound - lowerbound)
 * where diving is performed (0.0: no limit)
 *
 * \par MAXDIVEUBQUOTNOSOL
 * maximal UBQUOT when no solution was found yet (0.0: no limit)
 *
 * \par MAXDIVEAVGQUOTNOSOL
 * maximal AVGQUOT when no solution was found yet (0.0: no limit)
 *
 * \par BACKTRACK
 * use one level of backtracking if infeasibility is encountered?
 *
 * \par LPRESOLVEDOMCHGQUOT
 * parameter to control LP resolve dynamically based on this percentage of observed bound changes relative to all variables or
 * the LP branching candidates (integer variables with fractional solution values) from the last node where an LP has been solved.
 * This property has no effect when the LPSOLVEFREQ is set to 1.
 *
 * \par LPSOLVEFREQ
 * LP solve frequency for diveset, use a positive integer k to solve an LP at every k'th depth of the diving search (ie. 1 causes the
 * diveset to solve _all_ intermediate LPs) or 0 to only resolve the LP relaxation after propagation found at least a certain percentage
 * domain changes, see also the previous LPRESOLVEDOMCHGQUOT parameter.
 *
 * \par ONLYLPBRANCHCANDS
 * Set this property to TRUE if only LP branching candidates be considered for the execution of the diving algorithm instead of the slower but
 * more general constraint handler diving variable selection.
 *
 * \par DIVETYPES
 * bit mask that represents all supported dive types. Irrelevant if only LP branching candidates should be scored, otherwise, different
 * constraint handlers may ask the diveset if it supports their preferred divetype. See \ref type_heur.h for a list of
 * available dive types.
 *
 * @section DIVING_FUNDAMENTALCALLBACKS Fundamental callbacks of a diveset
 *
 * Only one callback is necessary to complete a diveset to guide the diving search performed:
 *
 * @subsection DIVESETGETSCORE
 *
 * The scoring callback expects a candidate variable and calculates a score value and a preferred direction. The selected
 * variable for diving will be one that _maximizes_ the score function provided by the diveset.
 * If the diveset should support more than one possible type of diving, it may use the divetype argument as a hint how
 * the caller of the score function (could be the diving algorithm itself or one of the constraint handlers that
 * implement diving variable selection) intends to perform the search.
 *
 * @section DIVING_FURTHERINFO Further information
 *
 * This is all there is to extend the SCIP set of diving heuristics by a new one. For further information, please see
 * diveset related methods in \ref type_heur.h, \ref pub_heur.h, \ref heuristics.h, and \ref heur_guideddiving.h or
 * other diving heuristics that implement diving through a diveset.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page RELAX How to add relaxation handlers
 *
 * SCIP provides specific support for LP relaxations of constraint integer programs. In addition, relaxation handlers,
 * also called relaxators, can be used to include other relaxations, e.g. Lagrange relaxations or semidefinite
 * relaxations. The relaxation handler manages the necessary data structures and calls the relaxation solver to generate dual
 * bounds and primal solution candidates.
 * \n
 * However, the data to define a single relaxation must either be extracted by the relaxation handler itself (e.g., from
 * the user defined problem data, the LP information, or the integrality conditions), or it must be provided by the constraint
 * handlers. In the latter case, the constraint handlers have to be extended to support this specific relaxation.
 * \n
 *
 * We now explain how users can add their own relaxation handlers using the C interface. As an example, look into the NLP
 * relaxation handler of the \ref RELAXATOR_MAIN "Relaxator example" (examples/Relaxator/src/relax_nlp.c). It is very easy to
 * transfer the C explanation to C++: whenever a method should be implemented using the SCIP_DECL_RELAX... notion,
 * reimplement the corresponding virtual member function of the abstract scip::ObjRelax wrapper base class.
 *
 * Additional documentation for the callback methods of a relaxation handler can be found in the file type_relax.h.
 *
 * Here is what you have to do to implement a relaxation handler:
 * -# Copy the template files src/scip/relax_xyz.c and src/scip/relax_xyz.h into files named "relax_myrelaxator.c"
 *    and "relax_myrelaxator.h".
 *    \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPincludeRelaxMyrelaxator() in order to include the relaxation handler into your SCIP instance,
 *    e.g, in the main file of your project (see, e.g., src/cmain.c in the Binpacking example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "myrelaxator".
 * -# Adjust the properties of the relaxation handler (see \ref RELAX_PROPERTIES).
 * -# Define the relaxation handler data (see \ref RELAX_DATA). This is optional.
 * -# Implement the interface methods (see \ref RELAX_INTERFACE).
 * -# Implement the fundamental callback methods (see \ref RELAX_FUNDAMENTALCALLBACKS).
 * -# Implement the additional callback methods (see \ref RELAX_ADDITIONALCALLBACKS). This is optional.
 *
 *
 * @section RELAX_PROPERTIES Properties of a Relaxation Handler
 *
 * At the top of the new file "relax_myrelaxator.c" you can find the relaxation handler properties,
 * which are given as compiler defines.
 * In the C++ wrapper class, you have to provide the relaxation handler properties by calling the constructor
 * of the abstract base class scip::ObjRelax from within your constructor.
 * The properties have the following meaning:
 *
 * \par RELAX_NAME: the name of the relaxation handler.
 * This name is used in the interactive shell to address the relaxation handler.
 * Additionally, if you are searching for a relaxation handler with SCIPfindRelax(), this name is looked up.
 * Names have to be unique: no two relaxation handlers may have the same name.
 *
 * \par RELAX_DESC: the description of the relaxation handler.
 * This string is printed as a description of the relaxation handler in the interactive shell.
 *
 * \par RELAX_PRIORITY: the priority of the relaxation handler.
 * During each relaxation solving round, the included relaxation handlers and the
 * price-and-cut loop for solving the LP relaxation are called in a predefined order, which is given by the priorities
 * of the relaxation handlers.
 * First, the relaxation handlers with non-negative priority are called in the order of decreasing priority.
 * Next, the price-and-cut loop for solving the LP relaxation is executed.
 * Finally, the relaxation handlers with negative priority are called in the order of decreasing priority.
 * \n
 * Usually, you will have only one relaxation handler in your application and thus only have to decide whether it should
 * be called before or after solving the LP relaxation. For this decision you should consider the complexity of
 * the relaxation solving algorithm and the impact of the resulting solution: if your relaxation handler provides a fast
 * algorithm that usually has a high impact (i.e. the relaxation is a good approximation of the
 * feasible region of the subproblem and the solution severely improves the dual bound), it should have a non-negative
 * priority.
 * \n
 * Note that for certain applications, it is useful to disable the LP relaxation and only use your custom relaxation.
 * This can easily be achieved by setting the "lp/solvefreq" parameter to -1.
 *
 * \par RELAX_FREQ: the default frequency for solving the relaxation.
 * The frequency defines the depth levels at which the relaxation solving method \ref RELAXEXEC is called.
 * For example, a frequency of 7 means, that the relaxation solving callback is executed for subproblems that are in depth
 * 0, 7, 14, ... of the branching tree. A frequency of 0 means that the callback is only executed at the root node, i.e.,
 * only the relaxation of the root problem is solved. A frequency of -1 disables the relaxation handler.
 *
 *
 *
 * @section RELAX_DATA Relaxation Handler Data
 *
 * Below the header "Data structures" you can find a struct which is called "struct SCIP_RelaxData".
 * In this data structure, you can store the data of your relaxation handler. For example, you should store the adjustable
 * parameters of the relaxation handler in this data structure.
 * If you are using C++, you can add relaxation handler data as usual as object variables to your class.
 * \n
 * Defining relaxation handler data is optional. You can leave the struct empty.
 *
 *
 * @section RELAX_INTERFACE Interface Methods
 *
 * At the bottom of "relax_myrelaxator.c", you can find the interface method SCIPincludeRelaxMyrelaxator(),
 * which also appears in "relax_myrelaxator.h".
 * SCIPincludeRelaxMyrelaxator() is called by the user, if (s)he wants to include the relaxation handler,
 * i.e., if (s)he wants to use the relaxation handler in his/her application.
 *
 * This method only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the relaxation handler. For this, you can either call
 * SCIPincludeRelax(),
 * or SCIPincludeRelaxBasic() since SCIP version 3.0. In the latter variant, \ref RELAX_ADDITIONALCALLBACKS "additional callbacks"
 * must be added via setter functions as, e.g., SCIPsetRelaxCopy(). We recommend this latter variant because
 * it is more stable towards future SCIP versions which might have more callbacks, whereas source code using the first
 * variant must be manually adjusted with every SCIP release containing new callbacks for relaxation handlers in order to compile.
 *
 * If you are using relaxation handler data, you have to allocate the memory for the data at this point.
 * You can do this by calling:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &relaxdata) );
 * \endcode
 * You also have to initialize the fields in struct SCIP_RelaxData afterwards.
 *
 * You may also add user parameters for your relaxation handler, see the method SCIPincludeConshdlrKnapsack() in
 * the \ref cons_knapsack.h "knapsack constraint handler" for an example of how to add user parameters.
 *
 *
 * @section RELAX_FUNDAMENTALCALLBACKS Fundamental Callback Methods of a Relaxation Handler
 *
 * The fundamental callback methods of the plugins are the ones that have to be implemented in order to obtain
 * an operational algorithm.
 * They are passed together with the relaxation handler itself to SCIP using SCIPincludeRelax() or SCIPincludeRelaxBasic(),
 * see @ref RELAX_INTERFACE.
 *
 *
 * Relaxation handler plugins have only one fundamental callback method, namely the \ref RELAXEXEC method.
 * This method has to be implemented for every relaxation handler; the other callback methods are optional.
 * In the C++ wrapper class scip::ObjRelax, the scip_exec() method (which corresponds to the \ref RELAXEXEC callback) is a virtual
 * abstract member function.
 * You have to implement it in order to be able to construct an object of your relaxation handler class.
 *
 * Additional documentation for the callback methods can be found in type_relax.h.
 *
 * @subsection RELAXEXEC
 * The RELAXEXEC is called in each relaxation solving round. It should solve the current
 * subproblem's relaxation.
 *
 * Note that, like the LP relaxation, the relaxation handler should only operate on variables for which the corresponding
 * column exists in the transformed problem. Typical methods called by a relaxation handler are SCIPconstructLP() and SCIPflushLP() to
 * make sure that the LP of the current node is constructed and its data can be accessed via calls to SCIPgetLPRowsData()
 * and SCIPgetLPColsData(), and SCIPseparateSol() to call the cutting plane separators for a given primal solution.
 *
 * The lowerbound computed by the relaxation should be returned in the lowerbound pointer. If the relaxation improves on the best
 * relaxation already computed (either <code>SCIPisRelaxSolValid()</code> returns FALSE, meaning that no relaxation solution
 * is available so far, or the lowerbound is larger than the value returned by <code>SCIPgetRelaxSolObj()</code>), then the primal
 * solution of the relaxation should be stored inside the data structures of SCIP with <code>SCIPsetRelaxSolVal()</code>,
 * <code>SCIPsetRelaxSolVals()</code> or <code>SCIPsetRelaxSolValsSol()</code>. If you set the values one by one, you will need to call
 * <code>SCIPmarkRelaxSolValid()</code> to inform SCIP that the solution is complete and valid. With the "includeslp" argument of
 * <code>SCIPsetRelaxSolVals()</code>, <code>SCIPsetRelaxSolValsSol()</code> and <code>SCIPmarkRelaxSolValid()</code> you need to tell SCIP
 * whether the relaxation included all lp rows. In this case, the solution will be enforced and, if feasible, added to the solution storage if the
 * lowerbound of this relaxator is larger than the LP's. You may also call SCIPtrySolFree() directly from the
 * relaxation handler to make sure that a solution is added to the solution storage if it is feasible, even if the relaxator does not
 * include the LP or another relaxator produced a stronger bound. Also note that when setting the values of the relaxation solution one by one,
 * the objective value of the relaxation solution will be updated incrementally. If the whole solution should be updated, using SCIPsetRelaxSolVals()
 * instead or calling SCIPclearRelaxSolVals() before setting the first value to reset the solution and the objective value to 0 may help the numerics.
 * Furthermore, there is a list of external branching candidates, that can be filled by relaxation handlers and constraint handlers,
 * allowing branching rules to take these candidates as a guide on how to split the problem into subproblems. If the relaxation
 * solution is enforced, the integrality constraint handler will add external branching candidates for the relaxation solution
 * automatically, but the relaxation handler can also directly call <code>SCIPaddExternBranchCand()</code>.
 *
 * Usually, the RELAXEXEC callback only solves the relaxation and provides a lower (dual) bound through the corresponding pointer and
 * possibly a solution through <code>SCIPsetRelaxSolVal()</code> calls.
 * However, it may also produce domain reductions, add additional constraints or generate cutting planes. It has the
 * following options:
 *  - detecting that the node is infeasible in the variable's bounds and can be cut off (result SCIP_CUTOFF)
 *  - adding an additional constraint and stating that the relaxation handler should not be called again on the same
 *    relaxation (result SCIP_CONSADDED)
 *  - reducing a variable's domain and stating that the relaxation handler should not be called again on the same
 *    relaxation (result SCIP_REDUCEDDOM)
 *  - adding a cutting plane to the LP and stating that the relaxation handler should not be called again on the same
 *    relaxation (result SCIP_SEPARATED)
 *  - stating that the relaxation handler solved the relaxation and should not be called again on the same relaxation
 *    (result SCIP_SUCCESS)
 *  - interrupting the solving process to wait for additional input, e.g., cutting planes (result SCIP_SUSPENDED)
 *  - stating that the separator was skipped (result SCIP_DIDNOTRUN).
 *
 * In the above criteria, "the same relaxation" means that the LP relaxation stayed unmodified. This means in particular
 * that no row has been added and no bounds have been modified. For example, changing the bounds of a variable will, as
 * long as it was a COLUMN variable, lead to a modification in the LP such that the relaxation handler is called again
 * after it returned with the result code SCIP_REDUCEDDOM. If the relaxation solution should be enforced, the relaxation
 * handler has to produce a new solution in this case which satisfies the updated LP. If a relaxation handler should only run
 * once per node to compute a lower bound, it should store the node of the last relaxation call itself and return
 * SCIP_DIDNOTRUN for subsequent calls in the same node.
 *
 *
 * @section RELAX_ADDITIONALCALLBACKS Additional Callback Methods of a Relaxation Handler
 *
 * The additional callback methods do not need to be implemented in every case. However, some of them have to be
 * implemented for most applications, they can be used, for example, to initialize and free private data.
 * Additional callbacks can either be passed directly with SCIPincludeRelax() to SCIP or via specific
 * <b>setter functions</b> after a call of SCIPincludeRelaxBasic(), see also @ref RELAX_INTERFACE.
 *
 * @subsection RELAXFREE
 *
 * If you are using relaxation handler data, you have to implement this method in order to free the relaxation handler
 * data. This can be done by the following procedure:
 *
 * @refsnippet{tests/src/relax/relax.c,SnippetRelaxFreeUnittest}
 *
 * If you have allocated memory for fields in your relaxation handler data, remember to free this memory
 * before freeing the relaxation handler data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 * @subsection RELAXINIT
 *
 * The RELAXINIT callback is executed after the problem is transformed.
 * The relaxation handler may, e.g., use this call to initialize its relaxation handler data.
 *
 * @subsection RELAXCOPY
 *
 * The RELAXCOPY callback is executed when a SCIP instance is copied, e.g. to
 * solve a sub-SCIP. By
 * defining this callback as
 * <code>NULL</code> the user disables the execution of the specified
 * relaxation handler for all copied SCIP instances. This may deteriorate the performance
 * of primal heuristics using sub-SCIPs.
 *
 * @subsection RELAXEXIT
 *
 * The RELAXEXIT callback is executed before the transformed problem is freed.
 * In this method, the relaxation handler should free all resources that have been allocated for the solving process in
 * RELAXINIT.
 *
 * @subsection RELAXINITSOL
 *
 * The RELAXINITSOL callback is executed when the presolving is finished and the branch-and-bound process is about to
 * begin. The relaxation handler may use this call to initialize its branch-and-bound specific data.
 *
 * @subsection REALXEXITSOL
 *
 * The RELAXEXITSOL callback is executed before the branch-and-bound process is freed.
 * The relaxation handler should use this call to clean up its branch-and-bound data.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page READER How to add file readers
 *
 * Mainly, file readers are called to parse an input file and generate a constraint integer programming model. They
 * create constraints and variables and activate variable pricers if necessary. However, they can also be called, for
 * example, to parse an input file containing information about a primal solution or fixing of variables. Besides that
 * it is possible to use some of them for writing (exporting) the problem in a specific format.  \n A complete list of
 * all file readers contained in this release can be found \ref FILEREADERS "here".
 *
 * Since a file reader is also responsible for writing a file, the user may
 * ask why the readers have not the name "filehandler". This name would
 * represent this plugin much better than the used one.
 * \n
 * The used name "readers" is historically grown. In the beginning of SCIP
 * there was no need to write/export problems. Therefore, the the plugin
 * name "readers" was best fitting for this plugin since only reading was essential.
 * It turned out, however, that it is quite nice to write/export certain subproblem during
 * the solving process mainly for debugging. Therefore, a writing callback
 * was added to the "readers" plugin.
 *
 * We now explain how users can add their own file readers.
 * Take the file reader for MIPs in IBM's Mathematical Programming System format (src/scip/reader_mps.c) as an example.
 * As all other default plugins, it is written in C. C++ users can easily adapt the code by using the scip::ObjReader wrapper
 * base class and implement the scip_...() virtual methods instead of the SCIP_DECL_READER... callback methods.
 *
 * Additional documentation for the callback methods of a file reader can be found in the file type_reader.h.
 *
 * Here is what you have to do to implement a file reader named "myreader" in C:
 * -# Copy the template files src/scip/reader_xyz.c and src/scip/reader_xyz.h into files named
 *    "reader_myreader.c" and "reader_myreader.h".
 *    \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPincludeReaderMyreader() in order to include the file reader into your SCIP instance,
 *    e.g., in the main file of your project (see, e.g., src/cmain.c in the Binpacking example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "myreader".
 * -# Adjust the \ref READER_PROPERTIES "properties of the file reader".
 * -# Define the \ref READER_DATA "file reader data". This is optional.
 * -# Implement the \ref READER_INTERFACE "interface methods".
 * -# Implement the \ref READER_FUNDAMENTALCALLBACKS "fundamental callback methods".
 * -# Implement the \ref READER_ADDITIONALCALLBACKS "additional callback methods". This is optional.
 *
 *
 * @section READER_PROPERTIES Properties of a File Reader
 *
 * At the top of the new file "reader_myreader.c" you can find the file reader properties.
 * These are given as compiler defines.
 * In the C++ wrapper class, you have to provide the file reader properties by calling the constructor
 * of the abstract base class scip::ObjReader from within your constructor.
 * The properties you have to set have the following meaning:
 *
 * \par READER_NAME: the name of the file reader.
 * This name is used in the interactive shell to address the file reader.
 * Additionally, if you are searching for a file reader with SCIPfindReader(), this name is looked up.
 * Names have to be unique: no two file readers may have the same name.
 *
 * \par READER_DESC: the description of the file reader.
 * This string is printed as a description of the file reader in the interactive shell.
 *
 * \par READER_EXTENSION: the file name extension of the file reader.
 * Each file reader is hooked to a single file name extension. It is automatically called if the user wants to read in a
 * file of corresponding name. The extensions of the different file readers have to be unique.
 * Note that the additional extension '.gz', '.z', or '.Z' (indicating a gzip compressed file) are ignored for assigning
 * an input file to a reader.
 * \n
 * It is not possible to hook up a (single) file reader with more than one file extension.
 * It is, however, not necessary to implement the same (parsing/writing) methods more than once, if you want to
 * support several file extension with the same parser. To do so look at the files reader_lp.c
 * and reader_rlp.c. Both support the LP format.
 *
 *
 * @section READER_DATA File Reader Data
 *
 * Below the header "Data structures" you can find a struct which is called "struct SCIP_ReaderData".
 * In this data structure, you can store the data of your file reader. For example, you should store the adjustable
 * parameters of the file reader in this data structure.
 * If you are using C++, you can add file reader data as usual as object variables to your class.
 * \n
 * Defining file reader data is optional. You can leave the struct empty.
 *
 *
 * @section READER_INTERFACE Interface Methods
 *
 * At the bottom of "reader_myreader.c", you can find the interface method SCIPincludeReaderMyreader(),
 * which also appears in "reader_myreader.h".
 * SCIPincludeReaderMyreader() is called by the user, if (s)he wants to include the reader,
 * i.e., if (s)he wants to use the reader in his/her application.
 *
 * This method only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the reader. For this, you can either call
 * SCIPincludeReader(),
 * or SCIPincludeReaderBasic() since SCIP version 3.0. In the latter variant, \ref READER_ADDITIONALCALLBACKS "additional callbacks"
 * must be added via setter functions as, e.g., SCIPsetReaderCopy(). We recommend this latter variant because
 * it is more stable towards future SCIP versions which might have more callbacks, whereas source code using the first
 * variant must be manually adjusted with every SCIP release containing new callbacks for readers in order to compile.
 *
 * If you are using file reader data, you have to allocate the memory for the data at this point.
 * You can do this by calling:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &readerdata) );
 * \endcode
 * You also have to initialize the fields in struct SCIP_ReaderData afterwards.
 *
 * You may also add user parameters for your file reader, see the method SCIPincludeReaderLp() in
 * src/scip/reader_lp.c for an example.
 *
 *
 * @section READER_FUNDAMENTALCALLBACKS Fundamental Callback Methods of a File Reader
 *
 * File reader plugins have no fundamental callback methods. This is due to
 * the fact that a file reader can be used for reading and/or writing a
 * file. A file reader is only useful if the reader method \ref READERREAD
 * and/or the writing method \ref READERWRITE is implemented.  One of these
 * methods should be implemented for every file reader; the other callback
 * methods \ref READERCOPY and \ref READERFREE are optional.  In the C++ wrapper class scip::ObjReader, the
 * scip_read() and scip_write() methods (which corresponds to the \ref
 * READERREAD and \ref READERWRITE callback) are virtual member
 * functions. At least one of them should be implemented.
 *
 * Additional documentation for the callback methods can be found in type_reader.h.
 *
 *
 * @section READER_ADDITIONALCALLBACKS Additional Callback Methods of a File Reader
 *
 * Additional callbacks can either be passed directly with SCIPincludeReader() to SCIP or via specific
 * <b>setter functions</b> after a call of SCIPincludeReaderBasic(), see also @ref READER_INTERFACE.
 *
 *
 * File reader plugins contain only additional callback methods, namely the methods \ref READERREAD,
 * \ref READERWRITE, \ref READERFREE, and \ref READERCOPY. Therefore, these are not needed to be implemented. However,
 * at least \ref READERREAD and/or \ref READERWRITE should be implemented (see notes
 * \ref READER_FUNDAMENTALCALLBACKS "above").
 *
 *
 * @subsection READERREAD
 *
 * The READERREAD callback is called when the user invokes SCIP to read in a file with file name extension
 * corresponding to the READER_EXTENSION property of the file reader. This is usually triggered by a call to the method
 * SCIPreadProb() or by an interactive shell command.
 * The READERREAD callback should parse the input file and perform the desired action, which usually means
 * generating a constraint integer programming model, adding a primal solution, fixing variables
 * in an existing model.
 * \n
 * Typical methods called by a file reader that is used to read/generate constraint
 * integer programming models are, for example,
 *
 * - creating an empty problem: SCIPcreateProb()
 * - creating the variables: SCIPcreateVar(), SCIPchgVarType(), SCIPchgVarLb(), SCIPchgVarUb(), SCIPaddVar(), and
 *   SCIPreleaseVar()
 * - modifying the objective function: SCIPchgVarObj() and SCIPsetObjsense().
 * - creating the constraints: SCIPcreateConsLinear(), SCIPaddCoefLinear(), SCIPchgLhsLinear(), SCIPchgRhsLinear(),
 *   SCIPaddCons(), and SCIPreleaseCons()
 *
 * Primal solutions can only be created for the transformed problem. Therefore, the user has to call SCIPtransformProb()
 * before (s)he reads in the file containing the solution and adds it to the solution pool via the method SCIPreadSol().
 *
 *
 * @subsection READERWRITE
 *
 * The READERWRITE callback is called when the user invokes SCIP to write a problem (original or transformed)
 * in the format the reader supports. This is only possible if this callback is implemented. To write the problem
 * all necessary information is given through the parameters of this callback method (see type_reader.h). This
 * information should be used to output the problem in the requested format. This callback method is usually
 * triggered by the call of the methods SCIPwriteOrigProblem(), SCIPwriteTransProblem(), SCIPprintOrigProblem(),
 * or SCIPprintTransProblem().
 * \n
 * A typical method called by a file reader which is used to write/export a constraint
 * integer programming model is SCIPinfoMessage(). This method outputs a given string into a file
 * or into stdout.
 * \n
 * For an example we refer to the writing method of the MPS reader (see reader_mps.c).
 *
 *
 * @subsection READERCOPY
 *
 * The READERCOPY callback is executed when a SCIP instance is copied, e.g. to solve a sub-SCIP. By defining this
 * callback as <code>NULL</code> the user disables the execution of the specified reader for all copied SCIP
 * instances. The question might arise why to copy that plugin. In case of debugging it is nice to be able to
 * write/display the copied instances. Since the reader is in charge of that, you might want to copy the plugin. Below
 * you see a standard implementation.
 *
 * @refsnippet{src/scip/reader_mps.c,SnippetReaderCopyMps}
 *
 * @subsection READERFREE
 *
 * If you are using file reader data, you have to implement this method in order to free the file reader data.
 * This can be done by the following procedure:
 *
 * @refsnippet{src/scip/reader_mps.c,SnippetReaderFreeMps}
 *
 * If you have allocated memory for fields in your file reader data, remember to free this memory
 * before freeing the file reader data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page DIALOG How to add dialogs
 *
 * SCIP comes with a command line shell which allows the user to read in problem instances, modify the solver's
 * parameters, initiate the optimization and display certain statistics and solution information. This shell consists
 * of dialogs, which are organized as a tree in SCIP. A node of this tree which is not a leaf represents a menu in
 * the shell and the children of this node correspond to the entries of this menu (which can again be menus). All
 * different dialogs are managed by a dialog handler, which, in particular, is responsible for executing the dialog
 * corresponding to the user's command in the shell. The concept of a dialog handler is different to that
 * of a constraint handler, which is used to manage objects of the same structure, see \ref CONS. In particular, SCIP
 * features only one dialog handler (dialog_default.h), whereas there may exist different constraint handlers.
 * \n
 * A complete list of all dialogs contained in this release can be found \ref DIALOGS "here".
 *
 * We now explain how users can extend the interactive shell by adding their own dialog.
 * We give the explanation for creating your own source file for each additional dialog. Of course, you can collect
 * different dialogs in one source file. Take src/scip/dialog_default.c, where all default dialog plugins are collected, as an
 * example.
 * As all other default plugins, the default dialog plugin and the template dialog are written in C. C++ users can easily
 * adapt the code by using the scip::ObjDialog wrapper base class and implement the scip_...() virtual methods instead of the
 * SCIP_DECL_DIALOG... callback methods.
 *
 * Additional documentation for the callback methods of a dialog can be found in the file type_dialog.h.
 *
 * Here is what you have to do to add a dialog (assuming your dialog is named "mydialog"):
 * -# Copy the template files src/scip/dialog_xyz.c and src/scip/dialog_xyz.h into files named "dialog_mydialog.c"
 *    and "dialog_mydialog.h".
 *    \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPincludeDialogMydialog() in order to include the dialog handler into your SCIP instance,
 *    e.g., in the main file of your project (see, e.g., src/cmain.c in the Binpacking example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "mydialog".
 * -# Adjust the \ref DIALOG_PROPERTIES "properties of the dialog".
 * -# Define the \ref DIALOG_DATA "dialog data". This is optional.
 * -# Implement the \ref DIALOG_INTERFACE "interface methods".
 * -# Implement the \ref DIALOG_FUNDAMENTALCALLBACKS "fundamental callback methods".
 * -# Implement the \ref DIALOG_ADDITIONALCALLBACKS "additional callback methods". This is optional.
 *
 *
 * @section DIALOG_PROPERTIES Properties of a Dialog
 *
 * At the top of the new file "dialog_mydialog.c" you can find the dialog properties.
 * These are given as compiler defines.
 * In the C++ wrapper class, you have to provide the dialog properties by calling the constructor
 * of the abstract base class scip::ObjDialog from within your constructor.
 * The properties you have to set have the following meaning:
 *
 * \par DIALOG_NAME: the name of the dialog.
 * In the interactive shell, this name appears as the command name of the dialog in the parent dialog.
 * Additionally, if you are searching an entry in a menu with SCIPdialogFindEntry(), this name is looked up.
 * Names within one menu have to be unique: no two dialogs in the same menu may have the same name.
 *
 * \par DIALOG_DESC: the description of the dialog.
 * This string is printed as a description of the dialog in the interactive shell if the additional
 * callback method \ref DIALOGDESC is not implemented.
 *
 * \par DIALOG_ISSUBMENU: whether the dialog is a (sub)menu.
 * This parameter states whether the dialog is a menu in the interactive shell, i.e., is the parent of further
 * dialogs.
 *
 *
 * @section DIALOG_DATA Dialog Data
 *
 * Below the header "Data structures" you can find a struct which is called "struct SCIP_DialogData".
 * In this data structure, you can store the data of your dialog.
 * If you are using C++, you can add dialog data as usual as object variables to your class.
 * \n
 * Defining dialog data is optional. You can leave the struct empty.
 *
 *
 * @section DIALOG_INTERFACE Interface Methods
 *
 * At the bottom of "dialog_mydialog.c" you can find the interface method SCIPincludeDialogMydialog(), which also appears
 * in "dialog_mydialog.h".
 * \n
 * This method only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the dialog, which can be done by the following lines of code:
 *
 * @refsnippet{src/scip/dialog_xyz.c,SnippetDialogAddXyz}
 *
 * Here "parentdialog" has to be an existing dialog which is defined to be a menu (see DIALOG_ISSUBMENU), e.g.,
 * the default root dialog. The method SCIPgetRootDialog() returns the root dialog.
 *
 * The interface method is called by the user, if (s)he wants to include the dialog, i.e., if (s)he wants to use the dialog in
 * his/her application.
 * Note that in order to be able to link the new dialog to an existing default dialog
 * (except the root dialog) it has to be included <b>after the
 * default dialogs plugin</b>, i.e., the SCIPincludeDialogMydialog() call has to occur after the
 * SCIPincludeDialogDefault() call. The SCIPincludeDialogDefault() method is called from within the SCIPincludeDefaultPlugins()
 * method. Therefore, it suffices to include your dialog plugins after you have called SCIPincludeDefaultPlugins().
 * In case you want to add a dialog to the <b>root dialog</b>, you just use the following
 * lines of code to get/create the root dialog.
 *
 * @refsnippet{tests/src/misc/snippets.c,SnippetDialogCreate}
 *
 * Therefore, in this case you do not have to worry about the calls of
 * SCIPincludeDialogDefault() and SCIPincludeDefaultPlugins() .
 *
 * If you are using dialog data, you have to allocate the memory for the data at this point.
 * You can do this by calling:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &dialogdata) );
 * \endcode
 * You also have to initialize the fields in struct SCIP_DialogData afterwards.
 *
 * Consider the following example. The user wants to add a "drawgraph" command to the root menu of SCIP.
 * (S)he copies the "dialog_xyz.c" and "dialog_xyz.h" files into files "dialog_drawgraph.c" and "dialog_drawgraph.h", respectively.
 * Then, (s)he puts the following code into the SCIPincludeDialogDrawgraph() method, compare SCIPincludeDialogDefault() in
 * src/scip/dialog_default.c:
 * @refsnippet{tests/src/misc/snippets.c,SnippetDialogInclude}
 *
 * Using this code, it is even possible to call SCIPincludeDialogDrawgraph() before including the default dialog plugins,
 * and you can also call it multiple times without causing inconsistencies in the dialog structure.
 *
 *
 * @section DIALOG_FUNDAMENTALCALLBACKS Fundamental Callback Methods of a Dialog
 *
 * Dialogs have only one fundamental callback method, namely the \ref DIALOGEXEC method.
 * This method has to be implemented for every dialog; the other callback methods are optional.
 * In the C++ wrapper class scip::ObjDialog, the scip_exec() method (which corresponds to the \ref DIALOGEXEC callback) is a virtual
 * abstract member function.
 * You have to implement it in order to be able to construct an object of your dialog class.
 *
 * Additional documentation for the callback methods can be found in type_dialog.h.
 *
 * @subsection DIALOGEXEC
 *
 * The DIALOGEXEC method is invoked, if the user selected the dialog's command name in the parent's menu. It should
 * execute what is stated in DIALOG_DESC, e.g., the display constraint handlers dialog should display information about
 * the constraint handlers included in SCIP, see src/scip/dialog_default.c.
 *
 * For typical methods called by the execution method, have a look at src/scip/dialog_default.c.
 *
 * The callback has to return which dialog should be processed next. This can be, for example, the root dialog
 * (SCIPdialoghdlrGetRoot()), the parent dialog (SCIPdialogGetParent()) or NULL, which stands for closing the interactive
 * shell.
 *
 *
 * @section DIALOG_ADDITIONALCALLBACKS Additional Callback Methods of a Dialog
 *
 * The additional callback methods do not need to be implemented in every case.
 * They can be used, for example, to free private data.
 *
 * @subsection DIALOGFREE
 *
 * If you are using dialog data, you have to implement this method in order to free the dialog data.
 * This can be done by the following procedure:
 * @refsnippet{tests/src/misc/snippets.c,SnippetDialogFree}
 *
 * If you have allocated memory for fields in your dialog data, remember to free this memory
 * before freeing the dialog data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 * @subsection DIALOGDESC
 *
 * This method is called when the help menu of the parent is displayed. It should output (usually a single line of)
 * information describing the meaning of the dialog.
 * \n
 * If this callback is not implemented, the description string of the dialog (DIALOG_DESC) is displayed instead.
 *
 * @subsection DIALOGCOPY
 *
 * The DIALOGCOPY callback is executed when a SCIP instance is copied, e.g. to solve a sub-SCIP. By defining this
 * callback as <code>NULL</code> the user disables the execution of this dialog for all copied SCIP instances. In
 * general there is no need to copy any dialog since it is most unlikely to start the interactive shell of the copied
 * instances.
 *
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page DISP How to add display columns
 *
 * While solving a constraint integer program, SCIP displays status information in a column-like fashion. The current
 * number of processed branching tree nodes, the solving time, and the relative gap between primal and dual bound are
 * examples of such display columns. There already exists a wide variety of display columns which can be activated or
 * deactivated on demand, see src/scip/disp_default.c. Additionally, the user can implement his/her own display columns
 * in order to track problem or algorithm specific values.
 * \n
 * A complete list of all displays contained in this release can be found \ref DISPLAYS "here".
 *
 * We now explain how users can add their own display columns.
 * We give the explanation for creating your own source file for each additional display column. Of course, you can collect
 * different additional display columns in one source file.
 * Take src/scip/disp_default.c, where all default display columns are collected, as an example.
 * As all other default plugins, the default display column plugins and the display column template are written in C.
 * C++ users can easily adapt the code by using the scip::ObjDisp wrapper base class and implement the scip_...() virtual methods
 * instead of the SCIP_DECL_DISP... callback methods.
 *
 *
 * Additional documentation for the callback methods of a display column can be found in the file type_disp.h.
 *
 * Here is what you have to do to implement a display column (assuming your display column is named "mydisplaycolumn"):
 * -# Copy the template files src/scip/disp_xyz.c and src/scip/disp_xyz.h into files named "disp_mydisplaycolumn.c"
 *    and "disp_mydisplaycolumn.h".
      \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPincludeDispMydisplaycolumn() in order to include the display column into your SCIP instance,
 *    e.g., in the main file of your project (see, e.g., src/cmain.c in the Binpacking example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "mydisplaycolumn".
 * -# Adjust the \ref DISP_PROPERTIES "properties of the display column".
 * -# Define the  \ref DISP_DATA "display column data". This is optional.
 * -# Implement the \ref DISP_INTERFACE "interface methods".
 * -# Implement the \ref DISP_FUNDAMENTALCALLBACKS "fundamental callback methods".
 * -# Implement the \ref DISP_ADDITIONALCALLBACKS "additional callback methods". This is optional.
 *
 *
 * @section DISP_PROPERTIES Properties of a Display Column
 *
 * At the top of the new file "disp_mydisplaycolumn.c" you can find the display column properties.
 * These are given as compiler defines.
 * In the C++ wrapper class, you have to provide the display column properties by calling the constructor
 * of the abstract base class scip::ObjDisp from within your constructor.
 * The properties you have to set have the following meaning:
 *
 * \par DISP_NAME: the name of the display column.
 * This name is used in the interactive shell to address the display column.
 * Additionally, if you are searching for a display column with SCIPfindDisp(), this name is looked up.
 * Names have to be unique: no two display columns may have the same name.
 *
 * \par DISP_DESC: the description of the display column.
 * This string is printed as a description of the display column in the interactive shell.
 *
 * \par DISP_HEADER: the header of the display column.
 * This string is printed as the header of the display column in the status information display.
 *
 * \par DISP_WIDTH: the width of the display column.
 * This parameter defines the width (number of characters) of the display column. The value of the parameter has to be
 * greater than or equal to the number of characters in the header string.
 *
 * \par DISP_PRIORITY: the priority of the display column.
 * The total width of status information lines is bounded by the parameter "display width". The display columns actually contained
 * in the status information display are selected in decreasing order of their priority. Furthermore, the user can force
 * columns to be displayed or not to be displayed in the status information display. For that, (s)he has to switch the value
 * of the display column's parameter "active" from "auto" (its default value) to "on" or "off", respectively.
 *
 * \par DISP_POSITION: the relative position of the display column.
 * In the status information display, the display columns are arranged from left to right in increasing order of their
 * relative position.
 *
 * \par DISP_STRIPLINE: the default for whether the display column should be separated with a line from its right neighbor.
 * This parameter states whether the display column should be separated with the string "|" from its right neighbor. In so
 * doing, the clearness of the status information display may improve.
 *
 * @section DISP_DATA Display Column Data
 *
 * Below the header "Data structures" you can find a struct which is called "struct SCIP_DispData".
 * In this data structure, you can store the data of your display column. For example, you should store the adjustable
 * parameters of the display column in this data structure.
 * If you are using C++, you can add display column data as usual as object variables to your class.
 * \n
 * Defining display column data is optional. You can leave the struct empty.
 *
 *
 * @section DISP_INTERFACE Interface Methods
 *
 * At the bottom of "disp_mydisplaycolumn.c" you can find the interface method SCIPincludeDispMydisplaycolumn(), which also
 * appears in "disp_mydisplaycolumn.h".
 * \n
 * This method only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the display column by calling the method
 * SCIPincludeDisp().
 *
 * The interface method is called by the user, if (s)he wants to include the display column, i.e., if (s)he wants to use the display column in his
 * application.
 *
 * If you are using display column data, you have to allocate the memory for the data at this point.
 * You can do this by calling:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &dispdata) );
 * \endcode
 * You also have to initialize the fields in struct SCIP_DispData afterwards.
 *
 * Although this is very uncommon, you may also add user parameters for your display column, see the method
 * SCIPincludeConshdlrKnapsack() in the \ref cons_knapsack.h "knapsack constraint handler" for an example.
 *
 *
 * @section DISP_FUNDAMENTALCALLBACKS Fundamental Callback Methods of a Display Column
 *
 * Display column plugins have only one fundamental callback method, namely the \ref DISPOUTPUT method.
 * This method has to be implemented for every display column; the other callback methods are optional.
 * In the C++ wrapper class scip::ObjDisp, the scip_output() method (which corresponds to the \ref DISPOUTPUT callback) is a virtual
 * abstract member function.
 * You have to implement it in order to be able to construct an object of your display column class.
 *
 * Additional documentation for the callback methods can be found in type_disp.h.
 *
 * @subsection DISPOUTPUT
 *
 * The DISPOUTPUT callback is called after each pricing loop during node processing and after a node has been processed.
 * In addition, at the root node, the callback is executed after each iteration of the price-and-cut loop.
 * It should write the display column information for the current node to a given output file stream.
 *
 * Typical methods called by a display column are, for example, SCIPdispLongint(), SCIPdispInt(), SCIPdispTime(), and
 * SCIPinfoMessage().
 *
 *
 * @section DISP_ADDITIONALCALLBACKS Additional Callback Methods of a Display Column
 *
 * The additional callback methods do not need to be implemented in every case.
 * They can be used, for example, to initialize and free private data.
 *
 * @subsection DISPCOPY
 *
 * The DISPCOPY callback is executed when a SCIP instance is copied, e.g. to solve a sub-SCIP. By defining this callback
 * as <code>NULL</code> the user disables the execution of the specified column. In general it is probably not needed to
 * implement that callback since the output of the copied instance is usually suppressed. In the other case or for
 * debugging the callback should be implement.
 *
 *
 * @subsection DISPFREE
 *
 * If you are using display column data, you have to implement this method in order to free the display column data.
 * This can be done by the following procedure:
 * @refsnippet{tests/src/misc/snippets.c,SnippetDispFree}
 *
 * If you have allocated memory for fields in your display column data, remember to free this memory
 * before freeing the display column data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 * @subsection DISPINIT
 *
 * The DISPINIT callback is executed after the problem is transformed.
 * The display column may, e.g., use this call to initialize its display column data.
 *
 * @subsection DISPEXIT
 *
 * The DISPEXIT callback is executed before the transformed problem is freed.
 * In this method, the display column should free all resources that have been allocated for the solving process in
 * \ref DISPINIT.
 *
 * @subsection DISPINITSOL
 *
 * The DISPINITSOL callback is executed when the presolving is finished and the branch-and-bound process is about to
 * begin. The display column may use this call to initialize its branch-and-bound specific data.
 *
 * @subsection DISPEXITSOL
 *
 * The DISPEXITSOL callback is executed before the branch-and-bound process is freed. The display column should use this
 * call to clean up its branch-and-bound specific data.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page EVENT How to add event handler
 *
 * While solving a constraint integer program, SCIP drops thousands of events such as SCIP_EVENTTYPE_VARFIXED (a
 * complete list of all events is given in type_event.h). These events can be caught and used to do something after a
 * certain event happens. Events can be used to speed up the solution process. For example, the set partitioning
 * constraint is only worth propagating if one of the involved variables is fixed. This can be detected by
 * catching the event SCIP_EVENTTYPE_VARFIXED. To be able to catch an event it is necessary to write an event handler
 * which defines what to do after a certain event was caught.
 *
 * We now explain how users can add their own event handlers. We give the explanation for creating your own
 * source file for each additional event handler. Of course, you can collect different event handlers in one source file
 * or you can put the event handler directly into the constraint handler.  In a \ref EVENTUSAGE "second step" we discuss
 * the usage of an event handler. This means how to catch and drop events. \ref EVENTTYPES "Finally", we give some notes on the existing
 * types of events.
 *
 * Take src/scip/cons_logior.c, where the event handler is directly included into the constraint handler. As all other
 * default plugins, the event handlers are written in C. C++ users can easily adapt the code by using the scip::ObjEventhdlr
 * wrapper base class and implement the scip_...() virtual methods instead of the SCIP_DECL_EVENT... callback methods.
 *
 * Additional documentation for the callback methods of an event handler can be found in the file type_event.h. There is
 * also an example written in C which deals with an event handler. You find this example in the directory
 * "examples/Eventhdlr/". An C++ example can be found within the TSP project (examples/TSP/src/EventhdlrNewSol.cpp).
 *
 * Here is what you have to do to implement an event handler (assuming your event handler is named "bestsol"):
 * -# Copy the template files src/scip/event_xyz.c and src/scip/event_xyz.h into files named "event_bestsol.c"
 *    and "event_bestsol.h".
      \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPincludeEventBestsol() in order to include the event handler into your SCIP instance,
 *    e.g., in the main file of your project (see, e.g., src/cmain.c in the Eventhdlr example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "bestsol".
 * -# Adjust the \ref EVENTHDLR_PROPERTIES "properties of the event handler".
 * -# Implement the \ref EVENT_INTERFACE "interface methods".
 * -# Implement the \ref EVENT_FUNDAMENTALCALLBACKS "fundamental callback methods".
 * -# Implement the \ref EVENT_ADDITIONALCALLBACKS "additional callback methods". This is optional.
 *
 *
 * @section EVENTHDLR_PROPERTIES Properties of a Event Handler
 *
 * At the top of the new file "event_bestsol.c" you can find the event handler properties.
 * These are given as compiler defines.
 * In the C++ wrapper class, you have to provide the event handler properties by calling the constructor
 * of the abstract base class scip::ObjEventhdlr from within your constructor.
 * The properties you have to set have the following meaning:
 *
 * \par EVENT_NAME: the name of the event handler.
 * This name has to be unique with respect to all other event handlers. If you are searching for an event handler with
 * SCIPfindEventhdlr(), this name is looked up.
 *
 * \par EVENT_DESC: the description of the event handler.
 * This string is printed as a description of the event handler.
 *
 * @section EVENTHDLR_DATA Event Handler Data
 *
 * Below the header "Data structures" you can find a struct which is called "struct SCIP_EventhdlrData".
 * In this data structure, you can store the data of your event handler. For example, you should store the adjustable
 * parameters of the event handler in this data structure.
 * If you are using C++, you can add event handler data as usual as object variables to your class.
 * \n
 * Defining event handler data is optional. You can leave the struct empty.
 *
 *
 * @section EVENT_INTERFACE Interface Methods
 *
 * At the bottom of "event_bestsol.c", you can find the interface method SCIPincludeEventBestsol(),
 * which also appears in "event_bestsol.h".
 * SCIPincludeEventBestsol() is called by the user, if (s)he wants to include the event handler,
 * i.e., if (s)he wants to use the event handler in his/her application.
 *
 * This method only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the event handler. For this, you can either call
 * SCIPincludeEventhdlr(),
 * or SCIPincludeEventhdlrBasic() since SCIP version 3.0. In the latter variant, \ref EVENT_ADDITIONALCALLBACKS "additional callbacks"
 * must be added via setter functions as, e.g., SCIPsetReaderCopy(). We recommend this latter variant because
 * it is more stable towards future SCIP versions which might have more callbacks, whereas source code using the first
 * variant must be manually adjusted with every SCIP release containing new callbacks for event handlers in order to compile.
 *
 * If you are using event handler data, you have to allocate the memory for the data at this point.
 * You can do this by calling:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &eventhdlrdata) );
 * \endcode
 * You also have to initialize the fields in struct SCIP_EventhdlrData afterwards.
 *
 * Although this is very uncommon, you may also add user parameters for your event handler, see the method
 * SCIPincludeConshdlrKnapsack() in the \ref cons_knapsack.h "knapsack constraint handler" for an example.
 *
 *
 * @section EVENT_FUNDAMENTALCALLBACKS Fundamental Callback Methods of a Event Handler
 *
 * The fundamental callback methods of the plugins are the ones that have to be implemented in order to obtain
 * an operational algorithm.
 * They are passed together with the event handler itself to SCIP using SCIPincludeEventhdlr() or SCIPincludeEventhdlrBasic(),
 * see @ref EVENT_INTERFACE.
 *
 *
 * Event handler plugins have only one fundamental callback method, namely the \ref EVENTEXEC method.  This method has
 * to be implemented for every event handler; the other callback methods are optional.  In the C++ wrapper class
 * scip::ObjEventhdlr, the scip_exec() method (which corresponds to the \ref EVENTEXEC callback) is a virtual abstract member
 * function.  You have to implement it in order to be able to construct an object of your event handler class.
 *
 * Additional documentation for the callback methods can be found in type_event.h.
 *
 * @subsection EVENTEXEC
 *
 * The EVENTEXEC callback is called after the requested event happened. Then the event handler can do some action in
 * reaction to the event.
 *
 * Typical the execution method sets a parameter to TRUE to indicate later in solving process that something happened
 * which should be analyzed further. In the \ref cons_knapsack.h "knapsack constraint handler" you find such a typical
 * example.
 *
 * @section EVENT_ADDITIONALCALLBACKS Additional Callback Methods of a Event Handler
 *
 * The additional callback methods do not need to be implemented in every case. However, some of them have to be
 * implemented for most applications, they can be used, for example, to initialize and free private data.
 * Additional callbacks can either be passed directly with SCIPincludeEventhdlr() to SCIP or via specific
 * <b>setter functions</b> after a call of SCIPincludeEventhdlrBasic(), see also @ref EVENT_INTERFACE.
 *
 * @subsection EVENTCOPY
 *
 * The EVENTCOPY callback is executed when a SCIP instance is copied, e.g. to solve a sub-SCIP. By defining this
 * callback as <code>NULL</code> the user disables the execution of the specified event handler for all copied SCIP
 * instances. Note that in most cases the event handler in the copied instance will be initialize by those objects (such
 * as constraint handlers or propagators) which need this event handler (see \ref cons_knapsack.h). In these cases the copy
 * callback can be ignored. In case of general events, such as a new best solution being found
 * (SCIP_EVENTTYPE_BESTSOLFOUND), you might want to implement that callback. The event handler example which you find
 * in the directory "examples/Eventhdlr/" uses that callback.
 *
 * @refsnippet{src/scip/event_softtimelimit.c,SnippetEventCopySofttimelimit}
 *
 * @subsection EVENTFREE
 *
 * If you are using event handler data, you have to implement this method in order to free the event handler data.
 * This can be done by the following procedure:
 *
 * @refsnippet{src/scip/event_softtimelimit.c,SnippetEventFreeSofttimelimit}
 *
 * If you have allocated memory for fields in your event handler data, remember to free this memory
 * before freeing the event handler data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 *
 * @subsection EVENTINIT
 *
 * The EVENTINIT callback is executed after the problem is transformed.
 * The event handler may, e.g., use this call to initialize its event handler data.
 *
 * @subsection EVENTEXIT
 *
 * The EVENTEXIT callback is executed before the transformed problem is freed.
 * In this method, the event handler should free all resources that have been allocated for the solving process in
 * \ref EVENTINIT.
 *
 * @subsection EVENTINITSOL
 *
 * The EVENTINITSOL callback is executed when the presolving is finished and the branch-and-bound process is about to
 * begin. The event handler may use this call to initialize its branch-and-bound specific data.
 *
 * @subsection EVENTEXITSOL
 *
 * The EVENTEXITSOL callback is executed before the branch-and-bound process is freed. The event handler should use this
 * call to clean up its branch-and-bound specific data.
 *
 * @section EVENTUSAGE Catching and Dropping Events
 *
 * After you have implemented the event handler, you have to tell SCIP for which events this event handler should be
 * used. This can be a general events, such as <code>SCIP_EVENTTYPE_BESTSOLFOUND</code>, or a variable event which is the most common
 * way.
 *
 * In case of a general (not variable) event you use the function SCIPcatchEvent() to attach to an event and
 * SCIPdropEvent() to release this event later.
 *
 * \code
 * SCIP_CALL( SCIPcatchEvent( scip, SCIP_EVENTTYPE_BESTSOLFOUND, eventhdlr, NULL, NULL) );
 * \endcode
 *
 * \code
 * SCIP_CALL( SCIPdropEvent( scip, SCIP_EVENTTYPE_BESTSOLFOUND, eventhdlr, NULL, NULL) );
 * \endcode
 *
 * If you want trigger some variable event, you use the method SCIPcatchVarEvent() to attach the variable event and
 * SCIPdropVarEvent() to drop it later.
 *
 * \code
 * SCIP_CALL( SCIPcatchVarEvent( scip, var, SCIP_EVENTTYPE_VARFIXED, eventhdlr, NULL, NULL) );
 * \endcode
 *
 * \code
 * SCIP_CALL( SCIPdropVarEvent( scip, var, SCIP_EVENTTYPE_VARFIXED, eventhdlr, NULL, NULL) );
 * \endcode
 *
 * @section EVENTTYPES Event types
 *
 * All available events are listed in type_event.h. There are atomic events such as <code>SCIP_EVENTTYPE_VARFIXED</code>
 * and combined events such as <code>SCIP_EVENTTYPE_VARCHANGED</code>. The events are encoded via bit masks. Each atomic
 * event has a unique power of two. This enables combination of the atomic events.
 *
 * SCIP only throws atomic events. However, an event handler might be interested in bunch of events. Through the
 * underlying bit masks it is possible to combine the atomic events. For example, <code>SCIP_EVENTTYPE_VARCHANGED</code>
 * is an event which combines the events <code>SCIP_EVENTTYPE_VARFIXED</code>, <code>SCIP_EVENTTYPE_VARUNLOCKED</code>,
 * <code>SCIP_EVENTTYPE_OBJCHANGED</code>, <code>SCIP_EVENTTYPE_GBDCHANGED</code>,
 * <code>SCIP_EVENTTYPE_DOMCHANGED</code>, and <code>SCIP_EVENTTYPE_IMPLADDED</code>.
 *
 * \code
 * #define SCIP_EVENTTYPE_VARCHANGED     (SCIP_EVENTTYPE_VARFIXED | SCIP_EVENTTYPE_VARUNLOCKED | SCIP_EVENTTYPE_OBJCHANGED
 *                                    | SCIP_EVENTTYPE_GBDCHANGED | SCIP_EVENTTYPE_DOMCHANGED | SCIP_EVENTTYPE_IMPLADDED)
 * \endcode
 *
 * Depending on the event type, the event offers different information. The methods which can be used to gain
 * access to this information are given in pub_event.h.
 *
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page NLPI How to add interfaces to nonlinear programming solvers
 *
 * NLPIs are used to interface a solver for nonlinear programs (NLP).
 * It is used, e.g., to solve convex relaxations of the problem or to find locally optimal solutions of
 * nonlinear relaxations or subproblems.
 * The NLPI has been designed such that it can be used independently from SCIP.
 *
 * While the NLPI itself corresponds to the solver interface, the NLPIPROBLEM corresponds to the
 * (solver specific) representation of a concrete nonlinear program.
 * An NLP is specified as a set of indexed variables with variable bounds, an objective function,
 * and a set of constraints, where each constraint is specified as a function which is restricted to lie
 * between given left and right hand sides (possibly infinite).
 * A function consists of a linear, quadratic, and general nonlinear part.
 * The linear and quadratic parts are specified via variable indices and coefficients, while the
 * general nonlinear part is specified via an expression tree.
 * That is, the user of the NLPI does not provide function evaluation callbacks but an algebraic representation of the NLP.
 * Interfaces for solvers that require function evaluations can make use of the NLPIORACLE, which
 * provides a set of methods to compute functions values, gradients, Jacobians, and Hessians for a given NLP.
 * See the interface to Ipopt for an example on how to use the NLPIORACLE.
 *
 * A complete list of all NLPIs contained in this release can be found \ref NLPIS "here".
 *
 * We now explain how users can add their own NLP solver interface.
 * Take the interface to Ipopt (src/nlpi/nlpi_ipopt.cpp) as an example.
 * Unlike most other plugins, it is written in C++.
 * Additional documentation for the callback methods of an NLPI, in particular for their input parameters,
 * can be found in the file type_nlpi.h.
 *
 * Here is what you have to do to implement an NLPI:
 * -# Copy the template files src/nlpi/nlpi_xyz.c and src/nlpi/nlpi_xyz.h into files named "nlpi_mynlpi.c"
 *    and "nlpi_mynlpi.h".
 *    \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPcreateNlpSolverMynlpi() in order to include the NLPI into your SCIP instance,
 *    e.g., in the main file of your project (see, e.g., src/cmain.c in the Binpacking example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "mynlpi".
 * -# Adjust the properties of the nlpi (see \ref NLPI_PROPERTIES).
 * -# Define the NLPI and NLPIPROBLEM data (see \ref NLPI_DATA).
 * -# Implement the interface methods (see \ref NLPI_INTERFACE).
 * -# Implement the fundamental callback methods (see \ref NLPI_FUNDAMENTALCALLBACKS).
 *
 *
 * @section NLPI_PROPERTIES Properties of an NLPI
 *
 * At the top of the new file "nlpi_mynlpi.c", you can find the NLPI properties.
 * These are given as compiler defines.
 * The properties you have to set have the following meaning:
 *
 * \par NLPI_NAME: the name of the NLP solver interface.
 * This name is used in the interactive shell to address the NLPI.
 * Additionally, if you are searching for an NLPI with SCIPfindNLPI(), this name is looked up.
 * Names have to be unique: no two NLPIs may have the same name.
 *
 * \par NLPI_DESC: the description of the NLPI.
 * This string is printed as a description of the NLPI in the interactive shell.
 *
 * \par NLPI_PRIORITY: the priority of the NLPI.
 * If an NLP has to be solved, an NLP solver has to be selected.
 * By default, the solver with the NLPI with highest priority is selected.
 * The priority of an NLPI should be set according to performance of the solver:
 * solvers that provide fast algorithms that are usually successful on a wide range of problems should have a high priority.
 * An easy way to list the priorities of all NLPIs is to type "display nlpis" in the interactive shell of SCIP.
 *
 * @section NLPI_DATA NLPI Data
 *
 * Below the header "Data structures" you can find structs which are called "struct SCIP_NlpiData" and "struct SCIP_NlpiProblem".
 * In this data structure, you can store the data of your solver interface and of a specific NLP problem.
 * For example, you could store a pointer to the block memory data structure in the SCIP_NlpiData data structure
 * and store a pointer to an NLPIoracle in the SCIP_NlpiProblem data structure.
 *
 * @section NLPI_INTERFACE Interface Methods
 *
 * At the bottom of "nlpi_mynlpi.c", you can find the interface method SCIPcreateNlpSolverXyz(),
 * which also appears in "nlpi_mynlpi.h".
 * \n
 * This method only has to be adjusted slightly.
 * It is responsible for creating an NLPI that contains all properties and callback methods of your
 * solver interface by calling the method SCIPnlpiCreate().
 * SCIPcreateNlpSolverXyz() is called by the user (e.g., SCIP), if (s)he wants to use this solver interface in his/her application.
 *
 * If you are using NLPI data, you have to allocate the memory for the data at this point.
 * You can do this by calling:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &nlpidata) );
 * \endcode
 * You also have to initialize the fields in struct SCIP_NlpiData afterwards. For freeing the
 * NLPI data, see \ref NLPIFREE.
 *
 *
 * @section NLPI_FUNDAMENTALCALLBACKS Fundamental Callback Methods of an NLPI
 *
 * The fundamental callback methods of the plugins are the ones that have to be implemented in order to obtain
 * an operational algorithm. Currently, all NLPI callbacks are fundamental.
 *
 * Additional documentation of the callback methods, in particular to their input parameters,
 * can be found in type_nlpi.h.
 *
 * @subsection NLPICOPY
 *
 * The NLPICOPY callback is executed if the plugin should be copied, e.g., when a SCIP instance is copied.
 *
 * @subsection NLPIFREE
 *
 * The NLPIFREE callback is executed if the NLP solver interface data structure should be freed, e.g., when a SCIP instance is freed.
 *
 * @subsection NLPIGETSOLVERPOINTER
 *
 * The NLPIGETSOLVERPOINTER callback can be used to pass a pointer to a solver specific data structure to the user.
 *
 * @subsection NLPICREATEPROBLEM
 *
 * The NLPICREATEPROBLEM callback is executed if a particular NLP problem is to be created.
 * The callback method should initialize a SCIP_NlpiProblem struct here that corresponds to an empty NLP.
 *
 * @subsection NLPIFREEPROBLEM
 *
 * The NLPIFREEPROBLEMPOINTER callback is executed if a particular NLP problem is to be freed.
 * The callback method should free a SCIP_NlpiProblem struct here.
 *
 * @subsection NLPIGETPROBLEMPOINTER
 *
 * The NLPIGETPROBLEMPOINTER callback can be used to pass a pointer to a solver specific data structure of the NLP to the user.
 *
 * @subsection NLPIADDVARS
 *
 * The NLPIADDVARS callback is executed if a set of variables with lower and upper bounds and names should be added to a particular NLP.
 * The callback method must add the new variables behind the previously added variables, if any.
 * If NULL is given for the lower bounds arguments, -infinity is assumed as lower bound for each new variable.
 * If NULL is given for the upper bounds arguments, +infinity is assumed as upper bound for each new variable.
 * It is also permitted to use NULL for the names argument.
 *
 * @subsection NLPIADDCONSTRAINTS
 *
 * The NLPIADDCONSTRAINTS callback is executed if a set of constraints should be added to a particular NLP.
 * Constraints are specified by providing left and right hand sides, linear and quadratic coefficients, expression trees, and constraint names.
 * All of these arguments are optional, giving NULL for left hand sides corresponds to -infinity, giving NULL for right hand sides corresponds to +infinity.
 *
 * @subsection NLPISETOBJECTIVE
 *
 * The NLPISETOBJECTIVE callback is executed to set the objective function of a particular NLP.
 *
 * @subsection NLPICHGVARBOUNDS
 *
 * The NLPICHGVARBOUNDS callback is executed to change the bounds on a set of variables of an NLP.
 *
 * @subsection NLPICHGCONSSIDES
 *
 * The NLPICHGCONSSIDES callback is executed to change the sides on a set of constraints of an NLP.
 *
 * @subsection NLPIDELVARSET
 *
 * The NLPIDELVARSET callback is executed to delete a set of variables from an NLP.
 * The caller provides an array in which for each variable it is marked whether it should be deleted.
 * In the same array, the method should return the new position of each variable in the NLP, or -1 if it was deleted.
 *
 * @subsection NLPIDELCONSSET
 *
 * The NLPIDELCONSSET callback is executed to delete a set of constraints from an NLP.
 * The caller provides an array in which for each constraint it is marked whether it should be deleted.
 * In the same array, the method should return the new position of each constraint in the NLP, or -1 if it was deleted.
 *
 * @subsection NLPICHGLINEARCOEFS
 *
 * The NLPICHGLINEARCOEFS callback is executed to change the coefficients in the linear part of the objective function or a constraint of an NLP.
 *
 * @subsection NLPICHGQUADCOEFS
 *
 * The NLPICHGQUADCOEFS callback is executed to change the coefficients in the quadratic part of the objective function or a constraint of an NLP.
 *
 * @subsection NLPICHGEXPRTREE
 *
 * The NLPICHGEXPRTREE callback is executed to replace the expression tree of the objective function or a constraint of an NLP.
 *
 * @subsection NLPICHGNONLINCOEF
 *
 * The NLPICHGNONLINCOEF callback is executed to change a single parameter in the (parametrized) expression tree of the objective function or a constraint of an NLP.
 *
 * @subsection NLPICHGOBJCONSTANT
 *
 * The NLPICHGOBJCONSTANT callback is executed to change the constant offset of the objective function of an NLP.
 *
 * @subsection NLPISETINITIALGUESS
 *
 * The NLPISETINITIALGUESS callback is executed to provide primal and dual initial values for the variables and constraints of an NLP.
 * For a local solver, these values can be used as a starting point for the search.
 * It is possible to pass a NULL pointer for any of the arguments (primal values of variables, dual values of variable bounds, dual values of constraints).
 * In this case, the solver should clear previously set starting values and setup its own starting point.
 *
 * @subsection NLPISOLVE
 *
 * The NLPISOLVE callback is executed when an NLP should be solved.
 * The solver may use the initial guess provided by \ref NLPISETINITIALGUESS as starting point.
 * The status of the solving process and solution can be requested by
 * \ref NLPIGETSOLSTAT, \ref NLPIGETTERMSTAT, \ref NLPIGETSOLUTION, and \ref NLPIGETSTATISTICS.
 *
 * @subsection NLPIGETSOLSTAT
 *
 * The NLPIGETSOLSTAT callback can be used to request the solution status (solved, infeasible, ...) after an NLP has been solved.
 *
 * @subsection NLPIGETTERMSTAT
 *
 * The NLPIGETTERMSTAT callback can be used to request the termination reason (normal, iteration limit, ...) after an NLP has been solved.
 *
 * @subsection NLPIGETSOLUTION
 *
 * The NLPIGETSOLUTION callback can be used to request the primal and dual solution values after an NLP solve.
 * The method should pass pointers to arrays of variable values to the caller.
 * It is possible to return only primal values for the variables, but no values for the dual variables, e.g., if a solver does not compute such values.
 *
 * @subsection NLPIGETSTATISTICS
 *
 * The NLPIGETSTATISTICS callback can be used to request the statistical values (number of iterations, time, ...) after an NLP solve.
 * The method should fill the provided NLPSTATISTICS data structure.
 *
 * <!-- NLPIGETWARMSTARTSIZE, NLPIGETWARMSTARTMEMO, NLPISETWARMSTARTMEMO are not documented,
      since they are currently not used, not implemented, and likely to change with a next version. -->
 *
 * @subsection NLPIGETINTPAR
 *
 * The NLPIGETINTPAR callback can be used to request the value of an integer valued NLP parameter.
 *
 * @subsection NLPISETINTPAR
 *
 * The NLPISETINTPAR callback is executed to set the value of an integer valued NLP parameter.
 *
 * @subsection NLPIGETREALPAR
 *
 * The NLPIGETREALPAR callback can be used to request the value of a real valued NLP parameter.
 *
 * @subsection NLPISETREALPAR
 *
 * The NLPISETREALPAR callback is executed to set the value of a real valued NLP parameter.
 *
 * @subsection NLPIGETSTRINGPAR
 *
 * The NLPIGETSTRINGPAR callback can be used to request the value of a string valued NLP parameter.
 *
 * @subsection NLPISETSTRINGPAR
 *
 * The NLPISETSTRINGPAR callback is executed to set the value of a string valued NLP parameter.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page EXPRINT How to add interfaces to expression interpreters
 *
 * An expression interpreter is a tool to compute point-wise and interval-wise the function values, gradients, and
 * derivatives of algebraic expressions which are given in the form of an expression tree.
 * It is used, e.g., by an NLP solver interface to compute Jacobians and Hessians for the solver.
 *
 * The expression interpreter interface in SCIP has been implemented similar to those of the LP solver interface (LPI).
 * For one binary, exactly one expression interpreter has to be linked.
 * The expression interpreter API has been designed such that it can be used independently from SCIP.
 *
 * A complete list of all expression interpreters contained in this release can be found \ref EXPRINTS "here".
 *
 * We now explain how users can add their own expression interpreters.
 * Take the interface to CppAD (\ref exprinterpret_cppad.cpp) as an example.
 * Unlike most other plugins, it is written in C++.
 *
 * Additional documentation for the callback methods of an expression interpreter, in particular for their input parameters,
 * can be found in the file \ref exprinterpret.h
 *
 * Here is what you have to do to implement an expression interpreter:
 * -# Copy the file \ref exprinterpret_none.c into a file named "exprinterpreti_myexprinterpret.c".
 *    \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Open the new files with a text editor.
 * -# Define the expression interpreter data (see \ref EXPRINT_DATA).
 * -# Implement the interface methods (see \ref EXPRINT_INTERFACE).
 *
 *
 * @section EXPRINT_DATA Expression Interpreter Data
 *
 * In "struct SCIP_ExprInt", you can store the general data of your expression interpreter.
 * For example, you could store a pointer to the block memory data structure.
 *
 * @section EXPRINT_INTERFACE Interface Methods
 *
 * The expression interpreter has to implement a set of interface method.
 * In your "exprinterpret_myexprinterpret.c", these methods are mostly dummy methods that return error codes.
 *
 * @subsection SCIPexprintGetName
 *
 * The SCIPexprintGetName method should return the name of the expression interpreter.
 *
 * @subsection SCIPexprintGetDesc
 *
 * The SCIPexprintGetDesc method should return a short description of the expression interpreter, e.g., the name of the developer of the code.
 *
 * @subsection SCIPexprintGetCapability
 *
 * The SCIPexprintGetCapability method should return a bitmask that indicates the capabilities of the expression interpreter,
 * i.e., whether it can evaluate gradients, Hessians, or do interval arithmetic.
 *
 * @subsection SCIPexprintCreate
 *
 * The SCIPexprintCreate method is called to create an expression interpreter data structure.
 * The method should initialize a "struct SCIP_ExprInt" here.
 *
 * @subsection SCIPexprintFree
 *
 * The SCIPexprintFree method is called to free an expression interpreter data structure.
 * The method should free a "struct SCIP_ExprInt" here.
 *
 * @subsection SCIPexprintCompile
 *
 * The SCIPexprintCompile method is called to initialize the data structures that are required to evaluate
 * a particular expression tree.
 * The expression interpreter can store data that is particular to a given expression tree in the tree by using
 * SCIPexprtreeSetInterpreterData().
 *
 * @subsection SCIPexprintFreeData
 *
 * The SCIPexprintFreeData method is called when an expression tree is freed.
 * The expression interpreter should free the given data structure.
 *
 * @subsection SCIPexprintNewParametrization
 *
 * The SCIPexprintNewParametrization method is called when the values of the parameters in a parametrized expression tree have changed.
 *
 * @subsection SCIPexprintEval
 *
 * The SCIPexprintEval method is called when the value of an expression represented by an expression tree should be computed for a point.
 *
 * @subsection SCIPexprintEvalInt
 *
 * The SCIPexprintEvalInt method is called when an interval that contains the range of an expression represented by an expression tree with respect to intervals for the variables should be computed.
 *
 * @subsection SCIPexprintGrad
 *
 * The SCIPexprintGrad method is called when the gradient of an expression represented by an expression tree should be computed for a point.
 *
 * @subsection SCIPexprintGradInt
 *
 * The SCIPexprintGradInt method is called when an interval vector that contains the range of the gradients of an expression represented by an expression tree with respect to intervals for the variables should be computed.
 *
 * @subsection SCIPexprintHessianSparsityDense
 *
 * The SCIPexprintHessianSparsityDense method is called when the sparsity structure of the Hessian matrix should be computed and returned in dense form.
 *
 * @subsection SCIPexprintHessianDense
 *
 * The SCIPexprintHessianDense method is called when the Hessian of an expression represented by an expression tree should be computed for a point.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page TABLE How to add statistics tables
 *
 * After solving a constraint integer program, SCIP can display statistics tables with information about, e.g., the solving time,
 * number of nodes, LP iterations or the number of calls and successes of different plugins via "display statistics" in the shell
 * or via SCIPprintStatistics() in the C-interface. There already exists a wide variety of statistics tables which can be activated
 * or deactivated on demand, see src/scip/table_default.c. Additionally, the user can implement his/her own statistics tables
 * in order to display problem or algorithm specific values.
 * \n
 * A complete list of all statistics tables contained in this release can be found \ref TABLES "here".
 *
 * We now explain how users can add their own statistics tables.
 * We give the explanation for creating your own source file for each additional statistics table. Of course, you can collect
 * different additional statistics tables in one source file.
 * Take src/scip/table_default.c, where all default statistics tables are collected, as an example.
 * As all other default plugins, the default statistics table plugins and the statistics table template are written in C.
 * C++ users can easily adapt the code by using the scip::ObjTable wrapper base class and implement the scip_...() virtual methods
 * instead of the SCIP_DECL_TABLE... callback methods.
 *
 *
 * Additional documentation for the callback methods of a statistics table can be found in the file type_table.h.
 *
 * Here is what you have to do to implement a statistics table (assuming your statistics table is named "mystatisticstable"):
 * -# Copy the template files src/scip/table_xyz.c and src/scip/table_xyz.h into files named "table_mystatisticstable.c"
 *    and "table_mystatisticstable.h".
 *    \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPincludeTableMystatisticstable() in order to include the statistics table into your SCIP instance,
 *    e.g., in the main file of your project (see, e.g., src/cmain.c in the Binpacking example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "mystatisticstable".
 * -# Adjust the \ref TABLE_PROPERTIES "properties of the statistics table".
 * -# Define the  \ref TABLE_DATA "statistics table data". This is optional.
 * -# Implement the \ref TABLE_INTERFACE "interface methods".
 * -# Implement the \ref TABLE_FUNDAMENTALCALLBACKS "fundamental callback methods".
 * -# Implement the \ref TABLE_ADDITIONALCALLBACKS "additional callback methods". This is optional.
 *
 *
 * @section TABLE_PROPERTIES Properties of a Statistics Table
 *
 * At the top of the new file "table_mystatisticstable.c" you can find the statistics table properties.
 * These are given as compiler defines.
 * In the C++ wrapper class, you have to provide the statistics table properties by calling the constructor
 * of the abstract base class scip::ObjTable from within your constructor.
 * The properties you have to set have the following meaning:
 *
 * \par TABLE_NAME: the name of the statistics table.
 * This name is used in the interactive shell to address the statistics table.
 * Additionally, if you are searching for a statistics table with SCIPfindTable(), this name is looked up.
 * Names have to be unique: no two statistic tables may have the same name.
 *
 * \par TABLE_DESC: the description of the statistics table.
 * This string is printed as a description of the statistics table in the interactive shell.
 *
 * \par TABLE_POSITION: the position of the statistics table.
 * In the statistics output, the statistics tables will be ordered by increasing position. Compare with the
 * default statistics tables in "table_default.c" to find a value which will give you the desired position
 * between the default statistics tables. If you give your table a negative position value, it will appear
 * before all SCIP statistcs, with a value larger than 20000 it will appear after all default statistics.
 *
 * \par TABLE_EARLIEST_STAGE: output of the statistics table is only printed from this stage onwards
 * The output routine of your statistics table will only be called if SCIP has reached this stage. For
 * example, the default table "tree" will only output information starting from SCIP_STAGE_SOLVING, because
 * there is no meaningful information available before, while the "presolver" table can already be called
 * in SCIP_STAGE_TRANSFORMED.
 *
 * @section TABLE_DATA Statistics Table Data
 *
 * Below the header "Data structures" you can find a struct which is called "struct SCIP_TableData".
 * In this data structure, you can store the data of your statistics table. For example, you should store the adjustable
 * parameters of the statistics table in this data structure.
 * If you are using C++, you can add statistics table data as usual as object variables to your class.
 * \n
 * Defining statistics table data is optional. You can leave the struct empty.
 *
 *
 * @section TABLE_INTERFACE Interface Methods
 *
 * At the bottom of "table_mystatisticstable.c" you can find the interface method SCIPincludeTableMystatisticstable(), which also
 * appears in "table_mystatisticstable.h".
 * \n
 * This method only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the statistics table by calling the method
 * SCIPincludeTable().
 *
 * The interface method is called by the user, if (s)he wants to include the statistics table, i.e., if (s)he wants to use the statistics table in an
 * application.
 *
 * If you are using statistics table data, you have to allocate the memory for the data at this point.
 * You can do this by calling:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &tabledata) );
 * \endcode
 * You also have to initialize the fields in struct SCIP_TableData afterwards.
 *
 * Although this is very uncommon, you may also add user parameters for your statistics table, see the method
 * SCIPincludeConshdlrKnapsack() in the \ref cons_knapsack.h "knapsack constraint handler" for an example.
 *
 *
 * @section TABLE_FUNDAMENTALCALLBACKS Fundamental Callback Methods of a Statistics Table
 *
 * Statistics table plugins have only one fundamental callback method, namely the \ref TABLEOUTPUT method.
 * This method has to be implemented for every display column; the other callback methods are optional.
 * In the C++ wrapper class scip::ObjTable, the scip_output() method (which corresponds to the \ref TABLEOUTPUT callback) is a virtual
 * abstract member function.
 * You have to implement it in order to be able to construct an object of your statistics table class.
 *
 * Additional documentation for the callback methods can be found in type_table.h.
 *
 * @subsection TABLEOUTPUT
 *
 * The TABLEOUTPUT callback is called whenever SCIP is asked to print statistics (because the user typed "display statistics"
 * in the shell or called SCIPprintStatistics()). In this callback, the table should print all of its information to the given file
 * (which may be NULL if the output should be printed to the console).
 *
 * Typical methods called by a statistics table are, for example, SCIPdispLongint(), SCIPdispInt(), SCIPdispTime(), and
 * SCIPinfoMessage().
 *
 *
 * @section TABLE_ADDITIONALCALLBACKS Additional Callback Methods of a Statistics Table
 *
 * The additional callback methods do not need to be implemented in every case.
 * They can be used, for example, to initialize and free private data.
 *
 * @subsection TABLECOPY
 *
 * The TABLECOPY callback is executed when a SCIP instance is copied, e.g. to solve a sub-SCIP. By defining this callback
 * as <code>NULL</code> the user disables the execution of the specified column. In general it is probably not needed to
 * implement that callback since the output of the copied instance is usually suppressed. In the other case or for
 * debugging the callback should be implement.
 *
 *
 * @subsection TABLEFREE
 *
 * If you are using statistics table data, you have to implement this method in order to free the statistics table data.
 * This can be done by the following procedure:
 * @refsnippet{tests/src/misc/snippets.c,SnippetTableFree}
 *
 * If you have allocated memory for fields in your statistics table data, remember to free this memory
 * before freeing the statistics table data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 * @subsection TABLEINIT
 *
 * The TABLEINIT callback is executed after the problem is transformed.
 * The statistics table may, e.g., use this call to initialize its statistics table data.
 *
 * @subsection TABLEEXIT
 *
 * The TABLEEXIT callback is executed before the transformed problem is freed.
 * In this method, the statistics table should free all resources that have been allocated for the solving process in
 * \ref TABLEINIT.
 *
 * @subsection TABLEINITSOL
 *
 * The TABLEINITSOL callback is executed when the presolving is finished and the branch-and-bound process is about to
 * begin. The statistics table may use this call to initialize its branch-and-bound specific data.
 *
 * @subsection TABLEEXITSOL
 *
 * The TABLEEXITSOL callback is executed before the branch-and-bound process is freed. The statistics table should use this
 * call to clean up its branch-and-bound specific data.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page BENDER How to add custom Benders' decomposition implementations
 *
 * Benders' decomposition is a very popular mathematical programming technique that is applied to solve structured
 * problems. Problems that display a block diagonal structure are particularly amenable to the application of Benders'
 * decomposition. Such problems are given by
 *
 * \f[
 *  \begin{array}[t]{rllclcl}
 *    \min & \displaystyle & c^{T}x & + & d^{T}y \\
 *         & \\
 *    subject \ to & \displaystyle & Ax & & & = & b \\
 *         & \\
 *         & \displaystyle & Tx & + & Hy & = & h \\
 *         & \\
 *         & & x & & & \in & \mathbb{Z}^{p}\times\mathbb{R}^{n - p}  \\
 *         & & & & y & \in & \mathbb{R}^{m} \\
 *  \end{array}
 * \f]
 *
 * The variables \f$x\f$ and \f$y\f$ are described as the first and second stage variables respectively. In the
 * classical use of Benders' decomposition, it is a requirement that the all second stage variables are continuous.
 * Extensions to the classical Benders' decomposition approach have permitted the use of more general second stage
 * problems.
 *
 * The application of Benders' decomposition to the above problem results in a subproblem, given by
 *
 * \f[
 *  \begin{array}[t]{rll}
 *    \min & \displaystyle & d^{T}y \\
 *         & \\
 *    subject \ to & \displaystyle & Hy  = h - T\bar{x} \\
 *         & \\
 *         & & y \in \mathbb{R}^{m} \\
 *  \end{array}
 * \f]
 *
 * where \f$\bar{x}\f$ is a solution vector of the first stage variables. As such, the subproblem is a problem only in
 * \f$y\f$. The dual solution to the subproblem, either an extreme point or extreme ray, is used to generate cuts that
 * are added to the master problem. Let \f$\lambda\f$ be the vector of dual variables associated with the set of
 * constraints from the subproblem. If, for a given \f$\bar{x}\f$, the subproblem is infeasible, then \f$\lambda\f$
 * corresponds to a dual ray and is used to produce the cut
 *
 * \f[
 *    0 \geq \lambda(h - Tx)
 * \f]
 *
 * which eliminates \f$\bar{x}\f$ from the master problem. If, for a given \f$\bar{x}\f$, the subproblem is feasible,
 * then \f$\lambda\f$ corresponds to a dual extreme point and is used to produce the cut
 *
 * \f[
 *    \varphi \geq \lambda(h - Tx)
 * \f]
 *
 * where \f$\varphi\f$ is an auxiliary variable added to the master problem as an underestimator of the optimal
 * subproblem objective function value.
 *
 * Given \f$\Omega^{p}\f$ and \f$\Omega^{r}\f$ as the sets of dual extreme points and rays of the subproblem,
 * respectively, the Benders' decomposition master problem is given by
 *
 * \f[
 *  \begin{array}[t]{rll}
 *    \min & \displaystyle & c^{T}x + \varphi \\
 *         & \\
 *    subject \ to & \displaystyle & Ax = b \\
 *         & \\
 *         & \displaystyle & \varphi \geq \lambda(h - Tx) \quad \forall \lambda \in \Omega^{r}\\
 *         & \\
 *         & \displaystyle & 0 \geq \lambda(h - Tx) \quad \forall \lambda \in \Omega^{r} \\
 *         & \\
 *         & & x \in \mathbb{Z}^{p}\times\mathbb{R}^{n - p}  \\
 *         & & \varphi \in \mathbb{R} \\
 *  \end{array}
 * \f]
 *
 * The Benders' decomposition framework of SCIP allows the user to provide a custom implementation of many aspects of
 * the Benders' decomposition algorithm. It is possible to implement multiple Benders' decompositions that work in
 * conjunction with each other. Such a situation is where you may have different formulations of the Benders'
 * decomposition subproblem.
 *
 * The current list of all Benders' decomposition implementations available in this release can be found \ref BENDERS
 * "here".
 *
 * We now explain how users can add their own Benders' decomposition implementations.
 * Take the default Benders' decomposition implementation (src/scip/benders_default.c) as an example.  Same as all other
 * default plugins, it is written in C. C++ users can easily adapt the code by using the scip::ObjBenders wrapper base
 * class and implement the scip_...() virtual methods instead of the SCIP_DECL_BENDERS... callback methods.
 *
 * Additional documentation for the callback methods of a Benders' decomposition implementation, in particular for the
 * input parameters, can be found in the file type_benders.h.
 *
 * Here is what you have to do to implement a custom Benders' decomposition:
 * -# Copy the template files src/scip/benders_xyz.c and src/scip/benders_xyz.h into files "benders_mybenders.c" and
 *  "benders_mybenders.h".
      \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPincludeBendersMybenders() in order to include the Benders' decomposition into your SCIP instance, e.g., in
 *  the main file of your project (see, e.g., src/cmain.c in the Binpacking example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "mybenders".
 * -# Adjust the properties of the Benders' decomposition (see \ref BENDERS_PROPERTIES).
 * -# Define the Benders' decomposition data (see \ref BENDERS_DATA). This is optional.
 * -# Implement the interface methods (see \ref BENDERS_INTERFACE).
 * -# Implement the fundamental callback methods (see \ref BENDERS_FUNDAMENTALCALLBACKS).
 * -# Implement the additional callback methods (see \ref BENDERS_ADDITIONALCALLBACKS).  This is optional.
 *
 *
 * @section BENDERS_PROPERTIES Properties of a Benders' decomposition
 *
 * At the top of the new file "benders_mybenders.c", you can find the Benders' decomposition properties.
 * These are given as compiler defines.
 * In the C++ wrapper class, you have to provide the Benders' decomposition properties by calling the constructor
 * of the abstract base class scip::ObjBenders from within your constructor.
 * The properties you have to set have the following meaning:
 *
 * \par BENDERS_NAME: the name of the Benders' decomposition.
 * This name is used in the interactive shell to address the Benders' decomposition.
 * Additionally, if you are searching for a Benders' decomposition with SCIPfindBenders(), this name is looked up.
 * Names have to be unique: no two Benders' decompositions may have the same name.
 *
 * \par BENDERS_DESC: the description of the Benders' decomposition.
 * This string is printed as a description of the Benders' decomposition in the interactive shell.
 *
 * \par BENDERS_PRIORITY: the priority of the Benders' decomposition.
 * During the enforcement and checking of solutions in src/scip/cons_benders.c and src/scip/cons_benderslp.c, every
 * active Benders' decompositions are called. The execution method of the Benders' decomposition calls each of the
 * subproblems and generates cuts from their solutions.  So the active Benders' decompositions are called in order of
 * priority until a cut is generated or feasibility is proven.
 * \n
 * The priority of the Benders' decomposition should be set according to the difficulty of solving the subproblems and
 * the generation of cuts. However, it is possible to prioritise the Benders' decompositions with respect to the
 * strength of the subproblem formulation and the resulting cuts.
 *
 * \par BENDERS_CUTLP: should Benders' decomposition cuts be generated during the enforcement of LP solutions.
 * This is a flag that is used by src/scip/cons_benders.c and src/scip/cons_benderslp.c to idicate whether the
 * enforcement of LP solutions should involve solving the Benders' decomposition subproblems. This should be set to TRUE
 * to have an exact solution algorithm. In the presence of multiple Benders' decomposition, it may be desired to enforce
 * the LP solutions for only a subset of those implemented.
 *
 * \par BENDERS_CUTRELAX: should Benders' decomposition cuts be generated during the enforcement of relaxation solutions.
 * This is a flag that is used by src/scip/cons_benders.c and src/scip/cons_benderslp.c to idicate whether the
 * enforcement of relaxation solutions should involve solving the Benders' decomposition subproblems. This should be
 * set to TRUE to have an exact solution algorithm. In the presence of multiple Benders' decomposition, it may be desired
 * to enforce the relaxation solutions for only a subset of those implemented. This parameter will only take effect if
 * external relaxation have been implemented.
 *
 * \par BENDERS_CUTPSEUDO: should Benders' decomposition subproblems be solved during the enforcement of pseudo solutions.
 * This is a flag that is used by src/scip/cons_benders.c and src/scip/cons_benderslp.c to indicate whether the
 * enforcement of pseudo solutions should involve solving the Benders' decomposition subproblems. This should be set to
 * TRUE, since not enforcing pseudo solutions could result in an error or suboptimal result. During the enforcement of
 * pseudo solutions, no cuts are generated. Only a flag to indicate whether the solution is feasible or if the LP should
 * be solved again is returned.
 *
 * \par BENDERS_SHAREAUXVARS: should this Benders' decomposition use the auxiliary variables from the highest priority
 * Benders' decomposition.
 * This parameter only takes effect if multiple Benders' decompositions are implemented. Consider the case that two Benders'
 * decompositions are implemented with different formulations of the subproblem. Since the subproblems in each of the
 * decomposition will have the same optimal solution, then it is useful to only have a single auxiliary variable for the
 * two different subproblems. This means that when an optimality cut is generated in the lower priority Benders'
 * decomposition, the auxiliary variable from the highest priority Benders' decomposition will be added to the right
 * hand side.
 *
 * @section BENDERS_DATA Benders' decomposition Data
 *
 * Below the header "Data structures" you can find a struct which is called "struct SCIP_BendersData".
 * In this data structure, you can store the data of your Benders' decomposition. For example, you should store the adjustable
 * parameters of the Benders' decomposition in this data structure. In a Benders' decomposition, user parameters for the
 * number of subproblems and an array to store the subproblem SCIP instances could be useful.
 * \n
 * Defining Benders' decomposition data is optional. You can leave the struct empty.
 *
 * @section BENDERS_INTERFACE Interface Methods
 *
 * At the bottom of "benders_mybenders.c", you can find the interface method SCIPincludeBendersMybenders(),
 * which also appears in "benders_mybenders.h"
 * SCIPincludeBendersMybenders() is called by the user, if (s)he wants to include the Benders' decomposition,
 * i.e., if (s)he wants to use the Benders' decomposition in his/her application.
 *
 * This method only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the Benders' decomposition. For this, you can either call SCIPincludeBenders(),
 * or SCIPincludeBendersBasic() since SCIP version 3.0. In the latter variant, \ref BENDERS_ADDITIONALCALLBACKS "additional callbacks"
 * must be added via setter functions as, e.g., SCIPsetBendersCopy(). We recommend this latter variant because
 * it is more stable towards future SCIP versions which might have more callbacks, whereas source code using the first
 * variant must be manually adjusted with every SCIP release containing new callbacks for Benders' decompositions in order to compile.
 *
 * If you are using Benders' decomposition data, you have to allocate the memory
 * for the data at this point. You can do this by calling:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &bendersdata) );
 * \endcode
 * You also have to initialize the fields in "struct SCIP_BendersData" afterwards. For freeing the
 * Benders' decomposition data, see \ref BENDERSFREE.
 *
 * You may also add user parameters for your Benders' decomposition, see \ref PARAM for how to add user parameters and
 * the method SCIPincludeBendersDefault() in src/scip/benders_default.c for an example.
 *
 * It is advised to disable presolving for the Benders' decomposition master problem by calling SCIPsetPresolving() with
 * the parameter SCIP_PARAMSETTING_OFF. Presolving should be disabled because reductions on the master problem could be
 * invalid since constraints have been transferred to the subproblems. It is not necessary to disable all presolving,
 * but care must be taken when it is used for the Benders' decomposition master problem.
 *
 * The Benders' decomposition constraint handler, see src/scip/cons_benders.c, includes a presolver for tightening the
 * bound on the auxiliary variables. This presolver can be enabled with by setting "presolving/maxround" to 1 and
 * "constraints/benders/maxprerounds" to 1. This presolver solves the Benders' decomposition subproblems without fixing
 * the master problem variables to find a lower bound for the auxiliary variable.
 *
 *
 * @section BENDERS_FUNDAMENTALCALLBACKS Fundamental Callback Methods of a Benders' decomposition
 *
 * The fundamental callback methods of the plugins are the ones that have to be implemented in order to obtain
 * an operational algorithm.
 * They are passed together with the Benders' decomposition itself to SCIP using SCIPincludeBenders() or SCIPincludeBendersBasic(),
 * see @ref BENDERS_INTERFACE.
 *
 * Benders' decomposition plugins have two callbacks, @ref BENDERSGETVAR and @ref BENDERSCREATESUB that must be implemented.
 *
 * Additional documentation for the callback methods, in particular to their input parameters,
 * can be found in type_benders.h.
 *
 * @subsection BENDERSGETVAR
 *
 * The BENDERSGETVAR callback provides a mapping between the master problem variables and their corresponding subproblem
 * variables, and vice versa. The parameters of this function include the variable for which the mapped variable is
 * desired and the problem number for the mapped variable. When requesting a subproblem variable for a given master
 * problem variable, the master variable is input with the appropriate subproblem index. If a master problem variable is
 * requested for a given subproblem variable, then the subproblem variable is input with the subproblem index given as
 * -1.
 *
 * An example of how to implement the mapping between the master and subproblem variables is shown by
 *
 * @refsnippet{src/scip/benders_default.c,SnippetBendersGetvarDefault}
 *
 * In the above code snippet, the hashmaps providing the mapping between the master and subproblem variables are
 * constructed in the <code>SCIP_STAGE_INIT</code> stage (see \ref BENDERSINIT).
 *
 * The requested variable is returned via the mappedvar parameter. There may not exist a corresponding master
 * (subproblem) variable for every input subproblem (master) variable. In such cases were no corresponding variable
 * exists, mappedvar must be returned as NULL.
 *
 * The mapped variables are retrieved by calling SCIPgetBendersMasterVar() and SCIPgetBendersSubproblemVar().
 *
 * The variable mapping must be created before <code>SCIP_STAGE_PRESOLVE</code> stage. This is because the variable
 * mapping is required for checking solutions found by heuristics during presolving.
 *
 * @subsection BENDERSCREATESUB
 *
 * The BENDERSCREATESUB callback is executed during the <code>SCIP_STAGE_INIT</code> stage. In this function, the
 * user must register the SCIP instances for each subproblem. The BENDERSCREATESUB callback is executed once for each
 * subproblem. The user registers a subproblem by calling SCIPbendersAddSubproblem().
 *
 * It is possible to build the SCIP instance for the subproblem during the execution of this callback. However, it may
 * be more convenient to build the subproblem instances during the problem creation stage of the master problem and
 * store the subproblem SCIP instances in SCIP_BendersData. This approach is used in src/scip/benders_default.c.
 *
 * @section BENDERS_ADDITIONALCALLBACKS Additional Callback Methods of a Benders' decomposition implementation
 *
 * The additional callback methods do not need to be implemented in every case. However, some of them have to be
 * implemented for most applications, they can be used, for example, to initialize and free private data.
 * Additional callbacks can either be passed directly with SCIPincludeBenders() to SCIP or via specific
 * <b>setter functions</b> after a call of SCIPincludeBendersBasic(), see also @ref BENDERS_INTERFACE.
 *
 * @subsection BENDERSFREE
 *
 * If you are using Benders' decomposition data (see \ref BENDERS_DATA and \ref BENDERS_INTERFACE), you have to
 * implement this method in order to free the Benders' decomposition data. This can be done by the following procedure:
 *
 * @refsnippet{src/scip/benders_default.c,SnippetBendersFreeDefault}
 *
 * If you have allocated memory for fields in your Benders' decomposition data, remember to free this memory
 * before freeing the Benders' decomposition data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 * @subsection BENDERSCOPY
 *
 * The BENDERSCOPY callback is executed when a SCIP instance is copied, e.g. to
 * solve a sub-SCIP. By
 * defining this callback as
 * <code>NULL</code> the user disables the execution of the specified
 * separator for all copied SCIP instances. This may deteriorate the performance
 * of primal heuristics using sub-SCIPs.
 *
 * If a user wishes to employ the Large Neighbourhood Benders' Search, it is necessary for the BENDERSCOPY callback to
 * be implemented. This is required because the sub-SCIP instances of large neighbourhood search heuristics can only
 * access the implemented Benders' decomposition if it is copied via the BENDERSCOPY callback.
 *
 * @subsection BENDERSINIT
 *
 * The BENDERSINIT callback is executed after the problem is transformed.
 * The Benders' decomposition implementation may, e.g., use this call to initialize its Benders' decomposition data. In
 * src/scip/benders_default.c BENDERSINIT is used to create the mapping between the master and subproblem variables.
 * The difference between the original and the transformed problem is explained in
 * "What is this thing with the original and the transformed problem about?" on \ref FAQ.
 *
 * @subsection BENDERSEXIT
 *
 * The BENDERSEXIT callback is executed before the transformed problem is freed.
 * In this method, the Benders' decomposition implementation should free all resources that have been allocated for
 * the solving process in BENDERSINIT.
 *
 * @subsection BENDERSINITPRE
 *
 * The BENDERSINITPRE callback is executed before the preprocessing is started, even if presolving is turned off.
 * The Benders' decomposition may use this call to initialize its presolving data before the presolving process begins.
 *
 * @subsection BENDERSEXITPRE
 *
 * The BENDERSEXITPRE callback is executed after the preprocessing has been finished, even if presolving is turned off.
 * The Benders' decomposition implementation may use this call, e.g., to clean up its presolving data.
 * Besides clean up, no time consuming operations should be done.
 *
 * @subsection BENDERSINITSOL
 *
 * The BENDERSINITSOL callback is executed when the presolving is finished and the branch-and-bound process is about to
 * begin. The Benders' decomposition implementation may use this call to initialize its branch-and-bound specific data.
 *
 * @subsection BENDERSEXITSOL
 *
 * The BENDERSEXITSOL callback is executed before the branch-and-bound process is freed. The Benders' decomposition
 * implementation should use this call to clean up its branch-and-bound data.
 *
 * @subsection BENDERSPRESUBSOLVE
 *
 * The BENDERSPRESUBSOLVE callback is provided to give the user an opportunity in each iteration to perform any setup
 * operations prior to solving the subproblems. This callback also allows the user to skip the subproblem solve for the
 * current iteration. In this case, the user must set the result parameter appropriately
 *  - the subproblem was not solved in this iteration. Other decompositions will be checked (SCIP_DIDNOTRUN).
 *  - a constraint has been added to the master problem. No other decompositions will be checked (SCIP_CONSADDED).
 *  - a cut has been added to the master problem. No other decompositions will be checked (SCIP_SEPARATED).
 *  - feasibility of the solution is reported to SCIP. Other decompositions will be checked (SCIP_FEASIBLE).
 *  - infeasibility of the solution is reported to SCIP. No other decompositions will be checked (SCIP_INFEASIBLE).
 *
 * @subsection BENDERSSOLVESUBCONVEX
 *
 * Two different subproblem solving functions are provide in the Benders' decomposition framework, BENDERSSOLVESUBCONVEX
 * and BENDERSSOLVESUB. These two solving functions are used in the two solving loops of the Benders' decomposition
 * framework. The first solving loop solves convex subproblems and convex relaxations of CIPs. The BENDERSSOLVESUBCONVEX
 * callback is executed only during the FIRST solving loop. Benders' cut generating methods suitable for convex
 * subproblems are executed during this solving loop. If a cut is found, then the second solve loop is not executed. If
 * your decomposition does not have any convex subproblems, then it is not necessary to implement the
 * BENDERSSOLVESUBCONVEX callback. However, it may be computationally beneficial to solve the convex relaxation of CIP
 * subproblems, such as the LP relaxation of a MIP subproblem.
 *
 * The second solve loop expects that the CIP subproblems are solved to optimality.
 *
 * If you implement the BENDERSSOLVESUBCONVEX callback, it is necessary to implement the BENDERSFREESUB callback.
 *
 * The objective function value after the subproblem solve and the result must be returned. The permissible results
 * are:
 *  - the subproblem was not solved in this iteration (SCIP_DIDNOTRUN)
 *  - the subproblem is solved and is feasible (SCIP_FEASIBLE)
 *  - the subproblem is solved and is infeasible (SCIP_INFEASIBLE)
 *  - the subproblem is solved and is unbounded (SCIP_UNBOUNDED)
 *
 * @subsection BENDERSSOLVESUB
 *
 * The BENDERSSOLVESUB is executed only during the SECOND solve loop. This callback function is used to solve CIP
 * subproblems. If your decomposition does not have any CIP subproblems, then it is not necessary to implement the
 * BENDERSSOLVESUB callback.
 *
 * If you implement the BENDERSSOLVESUB callback, it is necessary to implement the BENDERSFREESUB callback.
 *
 * The objective function value after the subproblem solve and the result must be returned. The permissible results
 * are:
 *  - the subproblem was not solved in this iteration (SCIP_DIDNOTRUN)
 *  - the subproblem is solved and is feasible (SCIP_FEASIBLE)
 *  - the subproblem is solved and is infeasible (SCIP_INFEASIBLE)
 *  - the subproblem is solved and is unbounded (SCIP_UNBOUNDED)
 *
 * @subsection BENDERSPOSTSOLVE
 *
 * The BENDERSPOSTSOLVE callback is executed after the subproblems have been solved and any required cuts have been
 * generated, but before the subproblems are freed. This callback provides the user an opportunity to interact the
 * subproblems at a global level. For example, the user is able to construct a solution to the original problem by
 * combining the solutions from the master problem and all subproblems.
 *
 * Additionally, the user is able to merge subproblems into the master problem during the execution of this callback.
 * The merging of subproblems into the master problem could be desired if it is too time consuming to satisfy the
 * feasibility of a subproblem or the appropriate cutting methods are not available for the provided subproblem. A list
 * of indicies of subproblems suitable for merging are given in the mergecands array. The first npriomergecands are the
 * merge candidates that must be merged into the master problem. If they are not, then the solution process will
 * terminate with an error. These merge candidates arise if a cut could not be generated due to numerical difficulties.
 * The remaining nmergecands - npriomergecands are subproblems that could be merged into the master problem if desired
 * by the user.
 *
 * @subsection BENDERSFREESUB
 *
 * The BENDERSFREESUB callback is executed to clean up the subproblems after the solving process and prepare them for
 * the next iteration. Typically, SCIPfreeTransform() is called for each subproblem to free the transformed problem.
 *
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page BENDERSCUT How to add custom Benders' decomposition cut generation methods
 *
 * The Benders' decomposition framework of SCIP allows the user to provide a custom implementation of cut generation
 * methods. The Benders' decomposition cut methods are linked to the Benders' decomposition implementations, not the
 * master problem SCIP instance. As such, you are able to have different Benders' decomposition cut methods for each
 * included Benders' decomposition.
 * The current list of all Benders' decomposition cut generation methods available in this release can be found
 * \ref BENDERSCUTS "here".
 *
 * We now explain how users can add their own Benders' decomposition cut methods.  Take the default Benders'
 * decomposition cut method (src/scip/benderscut_opt.c) as an example. This Benders' decomposition cut method generates
 * a classical optimality cut from the optimal dual solution to the convex relaxation of the Benders' decomposition
 * subproblem. Same as all other default plugins, it is written in C. C++ users can easily adapt the code by using the
 * scip::ObjBenderscut wrapper base class and implement the scip_...() virtual methods instead of the
 * SCIP_DECL_BENDERSCUT...  callback methods.
 *
 * Additional documentation for the callback methods of a Benders' decomposition cut methods, in particular for the
 * input parameters, can be found in the file type_benderscut.h.
 *
 * Here is what you have to do to implement a custom Benders' decomposition cut method:
 * -# Copy the template files src/scip/benderscut_xyz.c and src/scip/benderscut_xyz.h into files "benderscut_mybenderscut.c" and
 *  "benderscut_mybenderscut.h".
      \n
 *    Make sure to adjust your Makefile such that these files are compiled and linked to your project.
 * -# Use SCIPincludeBenderscutMybenderscut() in order to include the Benders' decomposition cut method into your SCIP
 *  instance, e.g., in the main file of your project (see, e.g., src/cmain.c in the Binpacking example).
 * -# Open the new files with a text editor and replace all occurrences of "xyz" by "mybenderscut".
 * -# Adjust the properties of the Benders' decomposition (see \ref BENDERSCUT_PROPERTIES).
 * -# Define the Benders' decomposition data (see \ref BENDERSCUT_DATA). This is optional.
 * -# Implement the interface methods (see \ref BENDERSCUT_INTERFACE).
 * -# Implement the fundamental callback methods (see \ref BENDERSCUT_FUNDAMENTALCALLBACKS).
 * -# Implement the additional callback methods (see \ref BENDERSCUT_ADDITIONALCALLBACKS).  This is optional.
 *
 *
 * @section BENDERSCUT_PROPERTIES Properties of a Benders' decomposition
 *
 * At the top of the new file "benderscut_mybenderscut.c", you can find the Benders' decomposition cut methods
 * properties. These are given as compiler defines.
 * In the C++ wrapper class, you have to provide the Benders' decomposition properties by calling the constructor
 * of the abstract base class scip::ObjBenderscut from within your constructor.
 * The properties you have to set have the following meaning:
 *
 * \par BENDERSCUT_NAME: the name of the Benders' decomposition cut method.
 * This name is used in the interactive shell to address the Benders' decomposition cut method.
 * Additionally, if you are searching for a Benders' decomposition cut method with SCIPfindBenderscut(), this name is
 * looked up. Names have to be unique: no two Benders' decomposition cut methods linked to the same Benders'
 * decomposition may have the same name.
 *
 * \par BENDERSCUT_DESC: the description of the Benders' decomposition cut method.
 * This string is printed as a description of the Benders' decomposition cut method in the interactive shell.
 *
 * \par BENDERSCUT_PRIORITY: the priority of the Benders' decomposition cut method.
 * In each of the Benders' decomposition subproblem solving loops, the included Benders' decomposition cut methods are
 * called in order of priority. The priority is important because once a cut is generated for a subproblem, no other
 * cuts are generated for that subproblem for that solving loop.
 * \n
 * The priority of the Benders' decomposition should be set according to the order in which the cut should be generated.
 * For example, the priority of the included cuts attempt to generation feasibility cuts (src/scip/benderscut_feas.c
 * and src/scip/benderscut_nogood.c) prior to attempting to generate optimality cuts.
 *
 * \par BENDERSCUT_LPCUT: Can this cut be applied to convex subproblems and convex relaxations of CIP subproblems?
 * Since the Benders' decomposition framework executes two different solving loops, one for convex subproblems and the
 * other for CIP subproblems, the Benders' decomposition cut methods must be partitioned by their suitability for each
 * subproblem type. If BENDERSCUT_LPCUT is set to TRUE, then this cut is only applied to convex subproblems and convex
 * relaxations of CIP subproblems.
 *
 * @section BENDERSCUT_DATA Benders' decomposition Data
 *
 * Below the header "Data structures" you can find a struct which is called "struct SCIP_BenderscutData".
 * In this data structure, you can store the data of your Benders' decomposition. For example, you should store the adjustable
 * parameters of the Benders' decomposition in this data structure. In a Benders' decomposition, user parameters for the
 * number of subproblems and an array to store the subproblem SCIP instances could be useful.
 * \n
 * Defining Benders' decomposition data is optional. You can leave the struct empty.
 *
 * @section BENDERSCUT_INTERFACE Interface Methods
 *
 * At the bottom of "benderscut_mybenderscut.c", you can find the interface method SCIPincludeBenderscutMybenderscut(),
 * which also appears in "benderscut_mybenderscut.h"
 * SCIPincludeBenderscutMybenderscut() is called by the user, if (s)he wants to include the Benders' decomposition,
 * i.e., if (s)he wants to use the Benders' decomposition in his/her application.
 *
 * This method only has to be adjusted slightly.
 * It is responsible for notifying SCIP of the presence of the Benders' decomposition. For this, you can either call SCIPincludeBenderscut(),
 * or SCIPincludeBenderscutBasic() since SCIP version 3.0. In the latter variant, \ref BENDERSCUT_ADDITIONALCALLBACKS "additional callbacks"
 * must be added via setter functions as, e.g., SCIPsetBenderscutCopy(). We recommend this latter variant because
 * it is more stable towards future SCIP versions which might have more callbacks, whereas source code using the first
 * variant must be manually adjusted with every SCIP release containing new callbacks for Benders' decompositions in order to compile.
 *
 * If you are using Benders' decomposition cut data, you have to allocate the memory
 * for the data at this point. You can do this by calling:
 * \code
 * SCIP_CALL( SCIPallocBlockMemory(scip, &benderscutdata) );
 * \endcode
 * You also have to initialize the fields in "struct SCIP_BenderscutData" afterwards. For freeing the
 * Benders' decomposition cut data, see \ref BENDERSCUTFREE.
 *
 * You may also add user parameters for your Benders' decomposition, see \ref PARAM for how to add user parameters and
 * the method SCIPincludeBenderscutOpt() in src/scip/benderscut_opt.c for an example.
 *
 *
 * @section BENDERSCUT_FUNDAMENTALCALLBACKS Fundamental Callback Methods of a Benders' decomposition cut method
 *
 * The fundamental callback methods of the plugins are the ones that have to be implemented in order to obtain
 * an operational algorithm.
 * They are passed together with the Benders' decomposition itself to SCIP using SCIPincludeBenderscut() or SCIPincludeBenderscutBasic(),
 * see @ref BENDERSCUT_INTERFACE.
 *
 * Benders' decomposition cut methods only one callback, @ref BENDERSCUTEXEC, that must be implemented.
 *
 * Additional documentation for the callback methods, in particular to their input parameters,
 * can be found in type_benderscut.h.
 *
 * @subsection BENDERSCUTEXEC
 *
 * The BENDERSCUTEXEC callback is called during the cut generation process within the Benders' decomposition subproblem
 * solving loop. This method must generate a cut for the given subproblem if the associated subsystem is not optimal
 * with respect to the checked master problem solution.
 *
 * When generating cuts, it is possible to store these within the SCIP_BendersData of the associated Benders'
 * decomposition. This is achieved by calling SCIPstoreBenderscutCons() (SCIPstoreBenderscutCut() if the Benders'
 * decomposition cut is added as a cutting plane instead as a constraint). The storing of cuts can be useful when using
 * the large neighbourhood Benders' search, where the cut generated in the sub-SCIP solve are transferred to the main
 * SCIP instance.
 *
 * The BENDERSCUTEXEC callback must return the result of the cut generation. The permissable results are:
 *  - if the Benders' cut was not run (SCIP_DIDNOTRUN).
 *  - if the Benders' cut was run, but there was an error in generating the cut (SCIP_DIDNOTFIND).
 *  - if the Benders' decomposition cut algorithm has not generated a constraint or cut (SCIP_FEASIBLE).
 *  - an additional constraint for the Benders' decomposition cut was generated (SCIP_CONSADDED).
 *  - a cutting plane representing the Benders' decomposition cut was generated (SCIP_SEPARATED).
 *
 * If the BENDERSCUTEXEC callback returns SCIP_DIDNOTFIND due to an error in the cut generation, if no other subproblems
 * generate a cut during the same iteration of the Benders' decomposition algorithm, then this could result in an
 * error. It is possible to avoid the error by merging the subproblem into the master problem (see \ref
 * BENDERSPOSTSOLVE).
 *
 * @section BENDERSCUT_ADDITIONALCALLBACKS Additional Callback Methods of a Separator
 *
 * The additional callback methods do not need to be implemented in every case. However, some of them have to be
 * implemented for most applications, they can be used, for example, to initialize and free private data.
 * Additional callbacks can either be passed directly with SCIPincludeBenderscut() to SCIP or via specific
 * <b>setter functions</b> after a call of SCIPincludeBenderscutBasic(), see also @ref BENDERSCUT_INTERFACE.
 *
 * @subsection BENDERSCUTFREE
 *
 * If you are using Benders' decomposition cut data (see \ref BENDERSCUT_DATA and \ref BENDERSCUT_INTERFACE), you have
 * to implement this method in order to free the Benders' decomposition cut data.
 *
 * If you have allocated memory for fields in your Benders' decomposition cut data, remember to free this memory
 * before freeing the Benders' decomposition data itself.
 * If you are using the C++ wrapper class, this method is not available.
 * Instead, just use the destructor of your class to free the member variables of your class.
 *
 * @subsection BENDERSCUTCOPY
 *
 * The BENDERSCUTCOPY callback is executed when a SCIP instance is copied, e.g. to
 * solve a sub-SCIP. By
 * defining this callback as
 * <code>NULL</code> the user disables the execution of the specified
 * separator for all copied SCIP instances. This may deteriorate the performance
 * of primal heuristics using sub-SCIPs.
 *
 * If the Benders' decomposition cuts are included by calling SCIPincludeBendersDefaultCuts() in the include method of
 * the Benders' decomposition implementation, such as SCIPincludeBendersDefault(), then it is not necessary to implement
 * BENDERSCUTCOPY. The copy method could be implemented to copy Benders' decomposition cut data from the original SCIP
 * instance to the sub-SCIP.
 *
 *
 * @subsection BENDERSCUTINIT
 *
 * The BENDERSCUTINIT callback is executed after the problem is transformed The Benders' decomposition cut method may,
 * e.g., use this call to initialize its Benders' decomposition cut data. The difference between the original and the
 * transformed problem is explained in "What is this thing with the original and the transformed problem about?" on \ref
 * FAQ.
 *
 * @subsection BENDERSCUTEXIT
 *
 * The BENDERSCUTEXIT callback is executed before the transformed problem is freed.  In this method, the Benders'
 * decomposition cut method should free all resources that have been allocated for the solving process in
 * BENDERSCUTINIT.
 *
 * @subsection BENDERSCUTINITSOL
 *
 * The BENDERSCUTINITSOL callback is executed when the presolving is finished and the branch-and-bound process is about to
 * begin. The Benders' decomposition implementation may use this call to initialize its branch-and-bound specific data.
 *
 * @subsection BENDERSCUTEXITSOL
 *
 * The BENDERSCUTEXITSOL callback is executed before the branch-and-bound process is freed. The Benders' decomposition
 * implementation should use this call to clean up its branch-and-bound data.
 *
 */
/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page CONF How to use conflict analysis
 *
 * Conflict analysis is a way to automatically use the information obtained from infeasible nodes
 * in the branch-and-bound tree.
 *
 * Once a node is declared infeasible, SCIP automatically tries to infer a constraint that explains the reason for the
 * infeasibility, in order to avoid similar situations later in the search.  This explanation essentially consists of a
 * constraint stating that at least one of its variables should have a bound different from the current infeasible node,
 * because the current setting led to infeasibility. Clearly, all variables that are fixed in the current infeasible
 * node would yield such a constraint (since this leads to infeasibility). The key point rather is to infer a "small"
 * constraint that does the same job. SCIP handles this by several heuristics. For this, SCIP sets up a
 * so-called (directed) conflict graph. The nodes in this graph correspond to bound changes of variables and an arc (@a
 * u, @a v) means that the bound change corresponding to @a v is based on the bound change of @a u. In general, a node
 * will have several ingoing arcs which represent all bound changes that have been used to infer (propagate) the bound
 * change in question. The graph also contains source nodes for each bound that has been changed during branching and an
 * artificial target node representing the conflict, i.e., the infeasibility. Essentially, SCIP heuristically constructs
 * a cut in this graph that involves few "branching nodes". For details on the techniques that SCIP uses,
 * we refer to the paper @par
 * Tobias Achterberg, Conflict Analysis in Mixed Integer Programming@n
 * Discrete Optimization, 4, 4-20 (2007)
 *
 * For conflict analysis to work well, the author of a \ref CONS "Constraint Handler" or a
 * \ref PROP "Propagator" has to implement three kinds of functionality:
 *
 * -# If one detects infeasibility, one should initiate conflict analysis, see \ref INITCONFS "below".
 * -# During propagation, one should call the right functions to fix variables.
 * -# One should implement the <em>so-called reverse propagation</em>.
 *
 * If this functionality is not implemented, SCIP will still work correctly, but cannot use the information of the constraint
 * handler or the propagator for conflict analysis. In this case, each bound reduction performed by the constraint
 * handler/propagator will be treated as if it had been a branching decision.
 *
 * @section INITCONFS Initiating Conflict Analysis
 *
 * If one detects infeasibility within propagation, one should do the following:
 * -# Call SCIPinitConflictAnalysis().
 * -# Inform SCIP about the variable bounds that are the reason for the detection of infeasibility
 * via the functions SCIPaddConflictLb(), SCIPaddConflictUb(), SCIPaddConflictBd(), or
 * SCIPaddConflictBinvar(). If there is more than one valid explanation of infeasibility, either one can be used.
 * Typically, smaller explanations tend to be better.
 * -# Call SCIPanalyzeConflict() from a propagator or SCIPanalyzeConflictCons() from a constraint
 * handler.
 *
 * This functionality allows SCIP to set up the conflict graph and perform a conflict analysis.
 *
 * @section Propagation
 *
 * When propagating variable domains, SCIP needs to be informed that the deduced variable bounds should be
 * used in conflict analysis. This can be done by the functions SCIPinferVarLbCons(),
 * SCIPinferVarUbCons(), and SCIPinferBinvarCons() for constraint handlers and SCIPinferVarLbProp(),
 * SCIPinferVarUbProp(), and SCIPinferBinvarProp() for propagators. You can pass one integer of
 * information that should indicate the reason of the propagation and can be used in reverse
 * propagation, see the next section.
 *
 * @section RESPROP Reverse Propagation
 *
 * Reverse Propagation is used to build up the conflict graph. Essentially, it provides an algorithm to detect the arcs
 * leading to a node in the conflict graph, i.e., the bound changes responsible for the new bound change deduced during
 * propagation. Reverse Propagation needs to be implemented in the RESPROP callback functions of
 * \ref CONSRESPROP "constraint handlers" or \ref PROPRESPROP "propagators".
 * These callbacks receive the following information: the variable which is under investigation (@p
 * infervar), the corresponding bound change (@p bdchgidx, @p boundtype), and the integer (@p inferinfo) that has been
 * supplied during propagation.
 *
 * One can use SCIPvarGetUbAtIndex() or SCIPvarGetLbAtIndex() to detect the bounds before or after the propagation that
 * should be investigated. Then the bounds that were involved should be passed to SCIP via SCIPaddConflictLb() and
 * SCIPaddConflictUb().  If there is more than one valid explanation of infeasibility, either one can be used.
 * Typically, smaller explanations tend to be better.
 *
 * Details and (more) examples are given in Sections @ref CONSRESPROP and @ref PROPRESPROP.
 *
 *
 * @section Example
 *
 * Consider the constraint handler @p cons_linearordering.c in the
 * \ref LOP_MAIN "linear ordering example"
 * (see @p example/LOP directory). This constraint handler propagates the equations \f$x_{ij} + x_{ji} =
 * 1\f$ and triangle inequalities \f$x_{ij} + x_{jk} + x_{ki} \leq 2\f$.
 *
 * When propagating the equation and <code>vars[i][j]</code> is fixed to 1, the constraint handler uses
 * \code
 *    SCIP_CALL( SCIPinferBinvarCons(scip, vars[j][i], FALSE, cons, i*n + j, &infeasible, &tightened) );
 * \endcode
 * Thus, variable <code>vars[j][i]</code> is fixed to 0 (@p FALSE), and it passes <code>i*n + j </code> as @p inferinfo.
 *
 * When it propagates the triangle inequality and both <code>vars[i][j]</code> and <code>vars[j][k]</code>
 * are fixed to 1, the constraint handler uses
 * \code
 *    SCIP_CALL( SCIPinferBinvarCons(scip, vars[k][i], FALSE, cons, n*n + i*n*n + j*n + k, &infeasible, &tightened) );
 * \endcode
 * Thus, in this case, variable  <code>vars[k][i]</code> is fixed to 0 and  <code>n*n + i*n*n +  j*n + k</code> is
 * passed as <code>inferinfo</code>.
 *
 * In reverse propagation, the two cases can be distinguished by @p inferinfo: if it is less than @p n*n,
 * we deal with an equation, otherwise with a triangle inequality. The constraint handler can then extract the
 * indices @p i, @p j (and @p k in the second case) from inferinfo.
 *
 * In the first case, it has to distinguish whether <code>vars[i][j]</code> is fixed to 0 or 1 &ndash;
 * by calling SCIPaddConflictLb()
 * or SCIPaddConflictUb(), respectively, with variable <code>vars[j][i]</code>. In the second case, it is clear that the only
 * possible propagation is to fix <code>vars[i][j]</code> to 0 when both <code>vars[k][i]</code> and <code>vars[j][k]</code>
 * are fixed to 1. It then calls
 * SCIPaddConflictLb() for both <code>vars[k][i]</code> and <code>vars[j][k]</code>.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page REOPT How to use reoptimization
 *
 * The reoptimization feature of SCIP can be used to solve a sequence of optimization problems \f$(P_{i})_{i \in I}\f$ with
 * \f[
 *    (P_i) \quad \min \{ c_i^T x \;|\; A^ix \geq b^i,\; x_{j} \in \mathbb{Z}\;\forall j \in \mathcal{I} \}
 * \f]
 * such that between two problems \f$P_i\f$ and \f$P_{i+1}\f$ the space of solutions gets restricted and/or the objective
 * function changes. To use reoptimization the user has to change the parameter <code>reoptimization/enable</code> to
 * <code>TRUE</code> before the solving process of the first problem of the sequence starts, i.e., in stage
 * <code>SCIP_STAGE_INIT</code> or <code>SCIP_STAGE_PROBLEM</code>. This can be done via the interactive shell or by
 * calling SCIPenableReoptimization(). In both cases SCIP changes some parameters and fixes them:
 * -# disable conflict analysis based on dual information
 * -# set the limit <code>maxorigsol</code> of stored solutions to zero because this is handled by a special solution tree provided
 *    by the reoptimization feature itself
 * -# disable restarts (<code>presolving/maxrestarts = 0</code>)
 * -# disable multi-aggegations (<code>presolving/donotmultaggr = TRUE</code>)
 * -# disable dual reductions within presolvers and propagators (<code>misc/allowdualreds = FALSE</code>)
 * -# disable propagation with current cutoff bound (<code>misc/allowobjprop = FALSE</code>)
 *
 * In contrast to the presolving and propagating methods that are using dual information, performing strong branching is
 * allowed. The bound tightenings resulting from strong branching are handeled in a special way. After changing the objective
 * function and solving the modified problem the feasible region that was pruned by strong branching will be reconstructed
 * within the tree.
 *
 * If the reoptimization feature is enabled SCIP tries to reuse the search tree, especially the search frontier at the end
 * of the solving process, to speed up the solving process of the following problems. Therefore, the current release
 * provides the branching rule <code>branch_nodereopt</code> to reconstruct the tree. SCIP triggers a restart of the
 * reoptimization, i.e., solving the problem from scratch, if
 *
 * -# the stored search tree is too large,
 * -# the objective functions changed too much, or
 * -# the last \f$n\f$ optimal solution are updated solution of previous runs.
 *
 * The thresholds to trigger a restart can be set by the user:
 *
 * -# <code>reoptimization/maxsavednodes</code>
 * -# <code>reoptimization/delay</code>
 * -# <code>reoptimization/forceheurrestart</code>
 *
 * Before SCIP discards all the stored information and solves the problem from scratch it tries to compress the search
 * tree. Therefore, the current release provides compression heuristics that try to find a good and much smaller
 * representation of the current search tree.
 *
 * After a problem in the sequence of optimization problems was solved, the objective function can be changed in two ways:
 * -# Using the provided reader <code>reader_diff</code> the objective function can be changed via using the interactive
 *    shell
 *    \code
 *    SCIP> read new_obj.diff
 *    \endcode
 *    or by calling SCIPreadDiff().
 * -# The objective function can be changed within the code. Therefore, the transformed problem needs to be freed by
 *    calling SCIPfreeReoptSolve(). Afterwards, the new objective function can be installed by calling
 *    SCIPchgReoptObjective().
 *
 * After changing the objective function the modified problem can be solved as usal.
 *
 * \note Currently, the compression heuristics used between two successive reoptimization runs only support pure binary
 * and mixed binary programs.
 *
 * For more information on reoptimization we refer to@par
 * Jakob Witzig@n
 * Reoptimization Techniques in MIP Solvers@n
 * Master's Thesis, Technical University of Berlin, 2014.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page CONCSCIP How to use the concurrent solving mode
 *
 * @section Overview
 *
 * In \SCIP 4.0 a new feature has been added that allows to run multiple \SCIP instances with different settings
 * on one problem in parallel. To use this feature \SCIP has to be compiled with an additional make option to
 * enable the threading functionality (e.g. TPI=tny, see \ref MAKE).
 * Then, a concurrent solve can be started by using the <code>concurrentopt</code> command instead of the <code>optimize</code> command
 * in the \SCIP shell, or by calling the interface function SCIPsolveParallel().
 * To configure the behavior of the concurrent solving mode there are new parameters in the category <code>concurrent/</code>
 * and <code>parallel/</code> which will be explained here shortly.
 *
 * @section CONTROLNTHREADS Controlling the number of threads
 *
 * The parameters <code>parallel/maxnthreads</code> and <code>parallel/minnthreads</code> can be used to configure the number of threads
 * that sould be used for solving. \SCIP will try to use the configured maximum number of threads. If the
 * problem that is currently read is too large \SCIP will automatically use fewer threads, but never
 * go below the configured minimum number of threads.
 *
 * @section USEEMPHSETTINGS Using emphasis settings
 *
 * The parameters <code>concurrent/scip.../prefprio</code> configure which concurrent solvers should be used.
 * The concurrent solver <code>scip</code> will use the same settings as the \SCIP instance configured by the user.
 * The other concurrent solvers, e.g. <code>scip-feas</code>, will load the corresponding emphasis setting.
 * The behavior of the prefprio parameter is as follows: If it is set to 1.0 for <code>scip-feas</code> and
 * <code>scip-opti</code>, and to 0.0 for every other concurrent solver, then the threads will be evenly
 * distributed between the two types <code>scip-feas</code> and <code>scip-opti</code>. An example: if 4 threads are used each of these concurrent
 * solvers will use 2 threads. If the <code>prefprio</code> for one solver is set to 0.33 and the other is set to 1.0, then the former will use 1 thread
 * and the latter will use 3 threads of the 4 available threads.
 *
 * @section CUSTOMCONCSOLVERS Running custom solvers
 *
 * To use custom settings for the concurrent solvers there is the parameter <code>concurrent/paramsetprefix</code>. If custom parameters
 * should be loaded by the concurrent solvers, then it must point to the folder where they are located (including a path separator at the end).
 * The parameter settings must be named after the concurrent solvers, e.g. if only the concurrent solver <code>scip</code> is used
 * they should be named <code>scip-1</code>, <code>scip-2</code>, <code>scip-3</code>. When different types of concurrent solvers are used the counter
 * starts at one for each of them, e.g. <code>scip-1</code> and <code>scip-feas-1</code>.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page DECOMP How to provide a problem decomposition
 *
 * Most mixed-integer programs have sparse constraint matrices in the sense that most columns and rows have only very few nonzero entries,
 * maybe except for a few outlier columns/rows with many nonzeros.
 * A decomposition identifies subproblems (subsets of rows and columns) that are only linked to each other via a set of linking rows and/or linking
 * columns, but are otherwise independent.
 * The special case of completely independent subproblems (with no linking rows and columns), for example, can be solved by solving
 * the much smaller subproblems and concatenating their optimal solutions.
 * This case has already been integrated into SCIP as a successful presolving technique (see @ref cons_components.c).
 * Another use of decomposition within SCIP is the @ref BENDDECF "Benders Decomposition framework".
 *
 * Since SCIP 7.0, it is easier to pass user decompositions to SCIP that can be used within Benders decomposition or by user algorithms.
 * This page introduces the struct SCIP_DECOMP and gives examples how to create and use it.
 *
 * @section DECOMP_OVERVIEW Overview
 *
 * In the following, we present decompositions of mixed-integer programs. However, the generalization to Constraint Integer Programs is straightforward.
 *
 * Concretely, for \f$k \geq 0\f$ we call a partition \f$\mathcal{D}=(D^{\text{row}},D^{\text{col}})\f$ of the rows and columns of the constraint matrix \f$A\f$ into \f$k + 1\f$ pieces each,
 *
 * \f[
 *   D^{\text{row}} = (D^{\text{row}}_{1},\dots,D^{\text{row}}_{k},L^{\text{row}}), \quad D^{\text{col}} = (D^{\text{col}}_{1},\dots,D^{\text{col}}_{k},L^{\text{col}})
 * \f]
 * a decomposition of \f$A\f$ if \f$D^{\text{row}}_{q} \neq \emptyset\f$, \f$D^{\text{col}}_{q} \neq \emptyset\f$ for \f$q \in \{1,\dots,k\}\f$ and if it holds for all \f$i\in D^{\text{row}}_{q_{1}}, j\in D^{\text{col}}_{q_{2}}\f$ that \f$a_{i,j} \neq 0 \Rightarrow q_{1} = q_{2}\f$.
 * The special rows \f$L^{\text{row}}\f$ and columns \f$L^{\text{col}}\f$, which may be empty, are called linking rows and linking columns, respectively.
 * In other words, the inequality system \f$A x \geq b\f$ can be rewritten
 * with respect to a decomposition \f$\mathcal{D}\f$ by a suitable permutation of the rows
 * and columns of \f$A\f$ as equivalent system
 * \f[
 *   \left(
 *   \begin{matrix}
 *     A_{[D^{\text{row}}_{1},D^{\text{col}}_{1}]} &
 *     0 &
 *     \cdots &
 *     0 &
 *     A_{[D^{\text{row}}_{1},L^{\text{col}}]}\\
 *     0 &
 *     A_{[D^{\text{row}}_{2},D^{\text{col}}_{2}]} &
 *     0 &
 *     0 &
 *     A_{[D^{\text{row}}_{2},L^{\text{col}}]}\\
 *     \vdots &
 *     0 &
 *     \ddots &
 *     0 &
 *     \vdots\\
 *     0 &
 *     \cdots &
 *     0 &
 *     A_{[D^{\text{row}}_{k},D^{\text{col}}_{k}]} &
 *     A_{[D^{\text{row}}_{k},L^{\text{col}}]}\\
 *     A_{[L^{\text{row}},D^{\text{col}}_{1}]} &
 *     A_{[L^{\text{row}},D^{\text{col}}_{2}]} &
 *     \cdots &
 *     A_{[L^{\text{row}},D^{\text{col}}_{k}]} &
 *     A_{[L^{\text{row}},L^{\text{col}}]}
 *   \end{matrix}
 *   \right)
 *   \left(
 *   \begin{matrix}
 *     x_{[D^{\text{col}}_{1}]}\\
 *     x_{[D^{\text{col}}_{2}]}\\
 *     \vdots\\
 *     x_{[D^{\text{col}}_{k}]}\\
 *     x_{[L^{\text{col}}]}
 *   \end{matrix}
 *   \right)
 *   \geq
 *   \left(
 *   \begin{matrix}
 *     b_{[D^{\text{row}}_{1}]}\\
 *     b_{[D^{\text{row}}_{2}]}\\
 *     \vdots\\
 *     b_{[D^{\text{row}}_{k}]}\\
 *     b_{[L^{\text{row}}]}
 *   \end{matrix}
 *   \right)
 * % A= \left(\begin{matrix}4&8&\frac{1}{2}\\\frac{3}{2}&4&1\\1&3&7\end{matrix}\right)
 * \f]
 * where we use the short hand syntax \f$A_{[I,J]}\f$ to denote
 * the \f$|I|\f$-by-\f$|J|\f$ submatrix that arises from the deletion of all entries
 * from \f$A\f$ except for rows \f$I\f$ and columns \f$J\f$,
 * for nonempty row
 * and column subsets \f$I\subseteq\{1,\dots,m\}\f$ and \f$J\subseteq\{1,\dots,n\}\f$.
 *
 *
 * @section DECOMP_USING Using a decomposition
 *
 * After passing one or more decompositions, see below, one can access all available decompositions with SCIPgetDecomps().
 * The labels can be obtained by calling SCIPdecompGetVarsLabels() and SCIPdecompGetConsLabels().
 * If some variables/constraints are not labeled, these methods will mark them as linking variables/constraints.
 * There are several methods to get more information about one decomposition, see @ref DecompMethods.
 *
 * A decomposition can be used to split the problem into several subproblems which, in general, are easier to solve.
 * For \f$q \in \{1,\dots,k\}\f$ the system
 * \f[
 *   A_{[D^{\text{row}}_{q},D^{\text{col}}_{q}]}\; x_{[D^{\text{col}}_{q}]} \geq b_{[D^{\text{row}}_{q}]}
 * \f]
 * is part of subproblem \f$q\f$, the handling of the linking variables/constraints depends on the chosen application context.
 * For example, in the heuristic @ref heur_padm.c several smaller subproblems are solved multiple times to get a feasible solution.
 * Also the @ref BENDDECF "Benders' decomposition framework" was extended with release 7.0 to use user decompositions.
 *
 * @section DECOMP_CREATION Creation via SCIP-API
 *
 * There are two different ways to provide a decomposition in SCIP.
 * It can be created with the SCIP-API or it can be read from a file.
 *
 * To create it with the API, the user must first create a decomposition with SCIPcreateDecomp() specifying
 * whether the decomposition belongs to the original or transformed problem and the number of blocks.
 * Then the variables and constraints can be assigned to one block or to the linking rows/columns by calling
 * SCIPdecompSetVarsLabels() and SCIPdecompSetConsLabels(), respectively.
 * To complete the decomposition or to ensure that it is internally consistent, SCIPcomputeDecompVarsLabels() or
 * SCIPcomputeDecompConsLabels() can be called.
 * Note that SCIPcomputeDecompVarsLabels() will ignore the existing variable labels and computes again the labels based on the constraint labels only;
 * SCIPcomputeDecompConsLabels() works in the same way and ignores the existing constraint labels.
 *
 * After the decomposition has been successfully created, it can be saved for later use in the DecompStore using SCIPaddDecomp().
 * Access to all decompositions in the DecompStore is possible with SCIPgetDecomps().
 *
 * @section DECOMP_READDEC Reading a decomposition from a file
 *
 * Alternatively, after a problem has been read, a related decomposition can be read from a dec-file.
 * Please refer to the @ref reader_dec.h "DEC file reader" for further information about the required file format.
 * Upon reading a valid dec-file, a decomposition structure is created, where the corresponding variable labels are inferred from the constraint labels, giving precedence to block over linking constraints.
 *
 * @section DECOMP_BENDERS Use for Benders
 *
 * If the variables should be labeled for the application of @ref BENDDECF "Benders' decomposition", the decomposition must be explicitly flagged by setting the parameter decomposition/benderslabels to TRUE.
 * With this setting, the variable's labeling takes place giving precedence to its presence in linking constraints over its presence in named blocks.
 *
 * @section DECOMP_TRANS Decomposition after problem transformation
 *
 * As the problem's constraints are constantly changing, or possibly deleted, during presolving, the constraints' labeling must be triggered again.
 * Therefore, SCIP automatically transforms all user decompositions at the beginning of the root node based on the variables' labels.
 *
 * @section DECOMP_STATS Decomposition statistics
 *
 * Further useful measures and statistics about the decomposition are computed within SCIPcomputeDecompStats().
 * When the labeling process is concluded, the following measures are computed and printed:
 * - the number of blocks;
 * - the number of linking variables and linking constraints;
 * - the size of the largest as well as the smallest block;
 * - the area score:
 * This score is also used by GCG to rank decompositions during the automatic detection procedure.
 * For a decomposition
 * \f$\mathcal{D}=(D^{\text{row}},D^{\text{col}})\f$,
 * the area score is defined as
 * \f[
 *   \text{areascore}(\mathcal{D}) = 1 - \frac{ \sum_{q=1}^k \lvert D^{\text{row}}_{q} \rvert
 *     \lvert D^{\text{col}}_{q} \rvert + n\lvert L^{\text{row}} \rvert + m\lvert L^{\text{col}} \rvert -
 *     \lvert L^{\text{row}} \rvert \lvert L^{\text{col}} \rvert }{mn}
 *   \enspace.
 * \f]
 * In the case of a mixed-integer program, the area score intuitively measures the coverage of the rearranged matrix by 0's.
 * Decompositions with few linking variables and/or constraints and many small blocks \f$A_{[D^{\text{row}}_{q},D^{\text{col}}_{q}]}\f$
 * will have an area score close to \f$1\f$, whereas coarse decompositions of a matrix have smaller area scores.
 * The trivial decomposition with a single block has the worst possible area score of 0.
 * - the modularity:
 * This measure is used to assess the quality of the community structure within a decomposition.
 * The modularity of the decomposition is computed as follows:
 * \f[
 * \begin{aligned}
 * \sum_{q=1}^{k} \dfrac{e_{q}}{m} \left(1-\dfrac{e_{q}}{m}\right),
 * \end{aligned}
 * \f]
 * where \f$e_{q}\f$ is the number of inner edges within block \f$q\f$ and \f$m\f$ is the total number of edges.
 * The presence of an inner edge is identified through the presence of a variable in a constraint,
 * both—the variable and the constraint—belonging to the same block.
 * - the block graph statistics: A block graph is constructed with the aim of depicting the connection between the different blocks in a decomposition through the existing linking variables in the constraints.
 * Note that the linking constraints are intentionally skipped in this computation.
 * \f$ G = (V,E) \f$ denotes a block graph, with vertex set \f$V\f$ and edge set \f$E\f$.
 * Each vertex in the graph represents a block in the decomposition; \f$V = \{v_{1},\dots,v_{k}\}\f$.
 * An edge \f$e = \{ v_{s},v_{t} \}\f$ is added to \f$G\f$, if and only if there exists a column \f$\ell \in L^{\text{col}}\f$, a row \f$i \in D^{\text{row}}_{s}\f$
 * and a row \f$j \in D^{\text{row}}_{t}\f$, such that \f$a_{i,\ell} \neq 0\f$ and \f$a_{j,\ell} \neq 0\f$.
 * From the constructed graph, the number of edges, articulation points and connected components are computed, together with the maximum and minimum degree.
 * Note that building the block graph can become computationally expensive with large and dense decompositions.
 * Thus, it is possible through a user parameter <code>decomposition/maxgraphedge</code> to define a maximum edge limit.
 * The construction process will be interrupted once this limit is reached, in which case only approximate estimations of the block graph statistics will be displayed and accompanied with a warning message.
 *
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/


/**@page BENDDECF How to use the Benders' decomposition framework
 *
 * Benders' decomposition is a very popular mathematical programming technique that is applied to solve structured
 * problems. Problems that display a block diagonal structure are particularly amenable to the application of Benders'
 * decomposition. In a purely mixed-integer linear setting, such problems are given by
 *
 * \f[
 *  \begin{array}[t]{rllclcl}
 *    \min & \displaystyle & c^{T}x & + & d^{T}y \\
 *         & \\
 *    \text{subject to} & \displaystyle & Ax & & & = & b \\
 *         & \\
 *         & \displaystyle & Tx & + & Hy & = & h \\
 *         & \\
 *         & & x & & & \in & \mathbb{Z}^{p}\times\mathbb{R}^{n - p}  \\
 *         & & & & y & \in & \mathbb{R}^{m} \\
 *  \end{array}
 * \f]
 *
 * The variables \f$x\f$ and \f$y\f$ are described as the first and second stage variables respectively. In the
 * classical use of Benders' decomposition, it is a requirement that the all second stage variables are continuous.
 * Extensions to the classical Benders' decomposition approach have permitted the use of more general second stage
 * problems.
 *
 * The application of Benders' decomposition to the above problem results in a subproblem, given by
 *
 * \f[
 *  \begin{array}[t]{rll}
 *    \min & \displaystyle & d^{T}y \\
 *         & \\
 *    \text{subject to} & \displaystyle & Hy  = h - T\bar{x} \\
 *         & \\
 *         & & y \in \mathbb{R}^{m} \\
 *  \end{array}
 * \f]
 *
 * where \f$\bar{x}\f$ is a solution vector of the first stage variables. As such, the subproblem is a problem only in
 * \f$y\f$. The dual solution to the subproblem, either an extreme point or extreme ray, is used to generate cuts that
 * are added to the master problem. Let \f$\lambda\f$ be the vector of dual variables associated with the set of
 * constraints from the subproblem. If, for a given \f$\bar{x}\f$, the subproblem is infeasible, then \f$\lambda\f$
 * corresponds to a dual ray and is used to produce the cut
 *
 * \f[
 *    0 \geq \lambda(h - Tx)
 * \f]
 *
 * which eliminates \f$\bar{x}\f$ from the master problem. If, for a given \f$\bar{x}\f$, the subproblem is feasible,
 * then \f$\lambda\f$ corresponds to a dual extreme point and is used to produce the cut
 *
 * \f[
 *    \varphi \geq \lambda(h - Tx)
 * \f]
 *
 * where \f$\varphi\f$ is an auxiliary variable added to the master problem as an underestimator of the optimal
 * subproblem objective function value.
 *
 * Given \f$\Omega^{p}\f$ and \f$\Omega^{r}\f$ as the sets of dual extreme points and rays of the subproblem,
 * respectively, the Benders' decomposition master problem is given by
 *
 * \f[
 *  \begin{array}[t]{rll}
 *    \min & \displaystyle & c^{T}x + \varphi \\
 *         & \\
 *    \text{subject to} & \displaystyle & Ax = b \\
 *         & \\
 *         & \displaystyle & \varphi \geq \lambda(h - Tx) \quad \forall \lambda \in \Omega^{r}\\
 *         & \\
 *         & \displaystyle & 0 \geq \lambda(h - Tx) \quad \forall \lambda \in \Omega^{r} \\
 *         & \\
 *         & & x \in \mathbb{Z}^{p}\times\mathbb{R}^{n - p}  \\
 *         & & \varphi \in \mathbb{R} \\
 *  \end{array}
 * \f]
 *
 * @section BENDERFRAMEWORK Overview
 *
 * In \SCIP 6.0 a Benders' decomposition framework has been implemented.
 *
 * The current framework can be used to handle a Benders Decomposition of CIPs of the form
 *
 * \f[
 *  \begin{array}[t]{rllclcl}
 *    \min & \displaystyle & c^{T}x & + & d^{T}y \\
 *    \text{subject to} & \displaystyle & g(x & , & y) & \in & [\ell,u] \\
 *         & & x & & & \in & X \\
 *         & & & & y & \in & Y \\
 *  \end{array}
 * \f]
 * when either
 * - the subproblem is convex: \f$g_i(x,y)\f$ convex on \f$X\times Y\f$ if \f$u_i<\infty\f$, \f$g_i(x,y)\f$ concave on \f$X\times Y\f$ if \f$\ell_i>-\infty\f$, and \f$Y=\mathbb{R}^m\f$, or
 * - the first stage variables are of binary type: \f$ X \subseteq \{0,1\}^n \f$.
 *
 * This framework can be used in four different
 * ways: inputting an instance in the SMPS file format, using the default Benders' decomposition implementation
 * (see src/scip/benders_default.c), implementing a custom Benders' decomposition plugin (see \ref BENDER), or by using
 * the Benders' decomposition mode of GCG.
 * An overview of how to use each of these methods will be provided in this section.
 *
 * @section BENDERSSMPS Inputting an instance in the SMPS file format.
 *
 * As part of the Benders' decomposition framework development, a reader for instances in the SMPS file format has been
 * implemented (see src/scip/reader_smps.c). The details regarding the SMPS file format can be found at:
 *
 * Birge, J. R.; Dempster, M. A.; Gassmann, H. I.; Gunn, E.; King, A. J. & Wallace, S. W.
 * A standard input format for multiperiod stochastic linear programs
 * IIASA, Laxenburg, Austria, WP-87-118, 1987
 *
 * In brief, the SMPS file format involves three different files:
 * - A core file (.cor): a problem instance in MPS format that is the core problem of a stochastic program.
 * - A time file (.tim): partitions the variables and constraints into the different stages of the stochastic program
 * - A stochastic file (.sto): describes the scenarios for each stage.
 *
 * The STO reader (see src/scip/reader_sto.c) constructs the stochastic program using the information from the core and
 * time files. By default, the STO reader will construct the deterministic equivalent of the stochastic program. A
 * parameter is provided "reading/sto/usebenders" that will inform the STO reader to apply Benders' decomposition to the
 * input stochastic program.
 *
 * @section BENDERSDEFAULT Using the default Benders' decomposition plugin.
 *
 * A default implementation of a Benders' decomposition plugin has been included in \SCIP 6.0 (see
 * src/scip/benders_default.c). In order to use the default plugin, the user must create SCIP instances of the master
 * problem and subproblems. The subproblems must also contain a copy of the master problem variables, since these are
 * fixed to the master problem solution values during the subproblem solving loop. These SCIP instances for the master
 * and subproblems are passed to the default plugin by calling SCIPcreateBendersDefault().
 *
 * The mapping between the master and subproblem variables is performed automatically. The default solving and cut
 * generation methods are applied to solve the input problem. It is important to note that the mapping between the
 * master and subproblem variables relies on the variable names. The variables of the subproblem corresponding to
 * master problem variables must have the same name in both problems.
 *
 * The CAP (stochastic capacitated facility location problem) example demonstrates the use of the default Benders'
 * decomposition implementation within a project.
 *
 * @section BENDERSCUSTOM Implementing a custom Benders' decomposition implementation.
 *
 * A custom Benders' decomposition requires the implementation of a Benders' decomposition plugin. The key aspects for
 * implementing a custom Benders' decomposition plugin are explained in \ref BENDER.
 *
 * @section BENDERSGCG Using the Benders' decomposition mode of GCG
 *
 * In GCG 3.0, a Benders' decomposition mode has been implemented. This mode takes the decomposition found by the
 * detection schemes of GCG and applies Benders' decomposition. Using GCG it is possible to apply Benders' decomposition
 * to general problem without having to manually construct the decompositions.
 *
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page OBJ Creating, capturing, releasing, and adding data objects
 *
 *  Data objects (variables, constraints, rows, ... ) are subject to reference counting
 *  to avoid expensive copying operations. This concept is similar to smart pointers.
 *  Creating such an object (e.g., by calling SCIPcreateVar()) will set the
 *  reference counter to one. Capturing an object (e.g., by calling SCIPcaptureVar()) increases the reference counter,
 *  releasing it (e.g., by calling SCIPreleaseVar()) decreases the counter. If the reference counter gets zero, the
 *  object will be destroyed automatically.
 *
 *  Remember that a created data object is automatically captured. If the user
 *  doesn't need the object anymore, (s)he has to call the object's release method.
 *
 *  When a data object is added to SCIP (e.g., by calling SCIPaddVar()) , it is captured again, such that a
 *  release call does not destroy the object. If SCIP doesn't need the object
 *  anymore, it is automatically released.
 *
 *  E.g., if the user calls
 * \code
 *  SCIPcreateVar(); // reference counter 1
 *  SCIPaddVar(); // reference counter 2
 *  SCIPreleaseVar(); // reference counter 1
 * \endcode
 *  the reference counter will be 1 afterwards, and the variable will be destroyed, if SCIP frees the problem.
 *  If the user wants to use this variable, e.g. for extracting statistics after SCIP was finished, the user must not call
 *  SCIPreleaseVar() right after adding the variable, but before terminating the program.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page PARAM How to add additional user parameters
 *
 *  Users may add their own parameters to SCIP by calling SCIPaddXyzParam(). Using
 *  this method, there are two possibilities for where to store the actual parameter value:
 *   - If the given valueptr is NULL, SCIP stores the parameter value internally, and
 *     the user can only access the value with the SCIPgetXyzParam() and
 *     SCIPsetXyzParam() calls.
 *   - If the given valueptr is not NULL, SCIP stores the parameter value at the given
 *     address, and the user can directly manipulate the value at this address.
 *     (S)he has to be careful with memory management in string parameters: when the
 *     SCIPaddStringParam() method is called, the given address must hold a char*
 *     pointer with value NULL. The default value is then copied into this pointer,
 *     allocating memory with BMSallocMemoryArray(). If the parameter is changed, the
 *     old string is freed with BMSfreeMemoryArray() and the new one is copied to a new
 *     memory area allocated with BMSallocMemoryArray(). When the parameter is freed,
 *     the memory is freed with BMSfreeMemoryArray().
 *     The user should not interfere with this internal memory management. Accessing
 *     the string parameter through the given valueptr is okay as long as it does not
 *     involve reallocating memory for the string.
 *
 *  In some cases, it is necessary to keep track of changes in a parameter.
 *  If this is the case, the user can define a method by the PARAMCHGD callback and use this method as
 *  the @c paramchgd parameter of the @c SCIPaddXyzParam() method, also giving a pointer to the data, which is
 *  needed in this method, as @c paramdata. If this method is not NULL, it is called every time
 *  the value of the parameter is changed.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page MEMORY Using the memory functions of SCIP
 *
 *  SCIP provides three ways for allocating memory:
 *  -# <b>block memory:</b> efficient handling of memory blocks of similar small sizes
 *  -# <b>buffer memory:</b> efficient handling of memory that needs to locally be allocated and freed
 *  -# <b>standard memory:</b> access to standard malloc/free
 *
 *  <em>Whenever possible, the first two should be used, because of reasons detailed below.</em>
 *
 *  In the following, we provide a brief description of these methods. We refer the reader to the dissertation of Tobias
 *  Achterberg for more details. We also present best practice models.
 *
 *  @section MEMBACK Background
 *
 *  The main goals for providing such particular methods are:
 * - <em>Accounting:</em> Using its own functions, SCIP knows the total size of memory allocated internally and can change its
 *   behavior: for instance, it can change to "memory saving mode" (using depth first search (DFS) and possibly do a garbage
 *   collection). It also allows for keeping a memory limit.
 * - <em>Speed:</em> SCIP often needs to allocate a very large number of small blocks of similar sizes (often powers of
 *   two). Depending on the operating system and compiler, the methods implemented in SCIP can be faster, since blocks
 *   of the same size are grouped together. Especially at the end of the 1990ies the standard malloc/free methods were
 *   quite ineffective. The experiments of Tobias Achterberg in 2007 show a speed improvement of 11 % when using block
 *   memory.
 * - <em>Efficiency:</em> Since blocks are groups in sizes, the blocks do not need to store their sizes within each
 *   block. In comparison, standard malloc/free stores the size using one word for each memory chunk. The price to pay
 *   is that one needs to pass the size to the methods that free a block. In any case, the methods in SCIP can save
 *   memory. Moreover, buffer memory is stored in similar places and not spread out, which also might help cache.
 * - <em>Debugging:</em> All of the possibilities provide methods to detect memory leaks. Together with tools like
 *   valgrind, this can be quite effective in avoiding such problems.
 *
 *  @n
 *  @section BLKMEM Block memory
 *
 *  SCIP offers its own block memory handling, which allows efficient handling of smaller blocks of memory in cases in
 *  which many blocks of the same (small) size appear. This is adequate for branch-and-cut codes in which small blocks
 *  of the same size are allocated and freed very often (for data structures used to store rows or branch-and-bound
 *  nodes). Actually, most blocks allocated within SCIP have small sizes like 8, 16, 30, 32, 64.  The idea is simple:
 *  There is a separate list of memory blocks for each interesting small size. When allocating memory, the list is
 *  checked for a free spot in the list; if no such spot exists, the list is enlarged. Freeing just sets the block to be
 *  available. Very large blocks are handled separately. See the dissertation of Tobias Achterberg for more details.
 *
 *  One important comment is that freeing block memory requires the size of the block in order to find the right list.
 *
 *  The most important functions are
 *  - SCIPallocBlockMemory(), SCIPallocBlockMemoryArray() to allocate memory
 *  - SCIPfreeBlockMemory(), SCIPfreeBlockMemoryArray() to free memory
 *
 *  An example code is:
 *  @refsnippet{tests/src/misc/snippets.c,SnippetArrayAllocAndFree}
 *  @n
 *
 *  @section BUFMEM Buffer memory
 *
 *  @subsection BUFMEMSTD Standard Buffer Memory
 *
 *  In addition to block memory, SCIP offers buffer memory. This should be used if memory is locally used within a
 *  function and freed within the same function. For this purpose, SCIP has a list of memory buffers that are reused for
 *  this purpose. In this way, a very efficient allocation/freeing is possible.
 *
 *  Note that the buffers are organized in a stack, i.e., freeing buffers in reverse order of allocation is faster.
 *
 *  The most important functions are
 *  - SCIPallocBuffer(), SCIPallocBufferArray() to allocate memory,
 *  - SCIPfreeBuffer(), SCIPfreeBufferArray() to free memory.
 *
 *  @subsection BUFMEMCLEAN Clean Buffer Memory
 *
 *  SCIP 3.2 introduced a new type of buffer memory, the <em>clean buffer</em>. It provides memory which is initialized to zero
 *  and requires the user to reset the memory to zero before freeing it. This can be used at performance-critical
 *  places where only few nonzeros are added to a dense array and removing these nonzeros individually is much faster
 *  than clearing the whole array. Similar to the normal buffer array, the clean buffer should be used for temporary memory
 *  allocated and freed within the same function.
 *
 *  The most important functions are
 *  - SCIPallocCleanBufferArray() to allocate memory,
 *  - SCIPfreeCleanBufferArray() to free memory.
 *
 *  @n
 *  @section STDMEM Standard memory
 *
 *  SCIP provides an access to the standard C functions @c malloc and @c free with the additional feature of tracking
 *  memory in debug mode. In this way, memory leaks can be easily detected. This feature is automatically activated in
 *  debug mode.
 *
 *  The most important functions are
 *  - SCIPallocMemory(), SCIPallocMemoryArray() to allocate memory,
 *  - SCIPfreeMemory(), SCIPfreeMemoryArray() to free memory.
 *
 *  @n
 *  @section MEMBESTPRACTICE Best Practice of Using Memory Functions
 *
 *  Since allocating and freeing memory is very crucial for the speed and memory consumption of a program, it is
 *  important to keep the following notes and recommendations in mind.
 *
 *  @subsection GEN General Notes
 *
 *  The following holds for all three types of memory functions:
 *  - In debug mode, the arguments are checked for overly large allocations (usually arising from a bug). Note that all
 *    arguments are converted to unsigned values of type @c size_t, such that negative sizes are converted into very
 *    large values.
 *  - The functions always allocate at least one byte and return non-NULL pointers if memory is available. In particular,
 *    freeing is always possible.
 *  - The freeing methods set the pointer to the memory to NULL.
 *  - Debugging can be supported by using the compiler flags @p NOBLKMEM=true, @p NOBUFMEM=true, @p NOBLKBUFMEM=true
 *    that turn off the usage of block memory, buffer memory, as well as block and buffer memory, respectively. Since,
 *    the internal block and buffer memory is freed at the end (leaving no memory leaks), turning them off allows tools
 *    like valgrind to find memory leaks.
 *  - Moreover, additional checks can be turned on by defining @p CHECKMEM in memory.c.
 *
 *  @n
 *  @subsection DOS Things to do ...
 *
 *  - Use buffer memory if your memory chunk can be allocated and freed within the same function.
 *  - Use buffer and block memory wherever possible, because of the reasons explained above.
 *  - Free memory in the reverse order in which it was allocated! For block and buffer memory this @b significantly
 *    speeds up the code.
 *  - Use as few memory allocations/freeing operations as possible, since these functions take a significant amount of time.
 *
 *  @n
 *  @subsection DONTS Things to avoid ...
 *
 *  - Avoid the usage of standard memory, since SCIP is unaware of the size used in such blocks.
 *  - Avoid reallocation with only slightly increased size, rather use a geometrically growing
 *    size allocation. SCIPcalcMemGrowSize() is one way to calculate new sizes.
 *  - Be careful with buffer memory reallocation: For single buffers, the memory is reallocated (using malloc); since
 *    the actual space might be larger than what was needed at allocation time, reallocation sometimes comes without
 *    extra cost. Note that reallocating block memory in most cases implies moving memory arround.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page DEBUG Debugging
 *
 *  If you need to debug your own code that uses SCIP, here are some tips and tricks:
 *
 *  - Use <b>asserts</b> in your code to show preconditions for the parameters, invariants and postconditions.
 *    Assertions are boolean expressions which inevitably have to evaluate to <code>TRUE</code>. Consider the
 *    following example:
 *
 *    @refsnippet{src/scip/cons_linear.c,SnippetDebugAssertions}
 *
 *    As you can see, both pointers and integers are checked for valid values at the beginning of the
 *    function <code>consCatchEvent()</code>. This is particularly important for, e.g., array indices like
 *    the variable <code>pos</code> in this example, where using the <code>consdata->vars[pos]</code>
 *    pointer could result in unexspected behaviour
 *    if the asserted precondition on <code>pos</code> were not matched and <code>pos</code> were an arbitrary index
 *    outside the array range.
 *
 *  - In order to activate assertions, use the <b>Debug mode</b> by compiling SCIP via
 *   \code
 *    cmake -DCMAKE_BUILD_TYPE=Debug
 *   \endcode
 *   or the Makefile equivalent
 *   \code
 *    make OPT=dbg
 *   \endcode and run the code. See \ref CMAKE and \ref MAKE for further information about compiler options for SCIP.
 *     As a rule of thumb, Spending only little extra time on
 *    asserting preconditions saves most of the time spent on debugging!
 *
 *  - Turn on <b>additional debug output</b> by adding the line
 *    \code
 *    #define SCIP_DEBUG
 *    \endcode
 *    at the top of SCIP files you want to analyze. This will output messages included in the code using
 *    <code>SCIPdebugMsg(scip, ...)</code> (or <code>SCIPdebugMessage()</code>), see \ref EXAMPLE_1.
 *    We recommend to also use <code>SCIPdebugMsg(scip, ...)</code> in your own code for being able to activate
 *    debug output in the same way.
 *  - If available on your system, we recommend to use a debugger like <code>gdb</code>
 *    to trace all function calls on the stack,
 *    display values of certain expressions, manually break the running code, and so forth.
 *  - If available on your system, you can use software like <a href="http://valgrind.org">valgrind</a> to check for uninitialized
 *    values or segmentation faults.
 *  - For checking the usage of SCIP memory, you can use
 *    <code>SCIPprintMemoryDiagnostic()</code>. This outputs memory that is currently in use,
 *    which can be useful after a <code>SCIPfree()</code> call.
 *  - If there are memory leaks for which you cannot detect the origin, you can recompile your code with the option <code>cmake -DNOBLKBUFMEM=on</code>
 *    (or <code>make NOBLKBUFMEM=true</code> if you are using the Makefile system.
 *    Also for the Makefile system, do not forget to clean your code before with <code>make OPT=... LPS=... clean</code>)
 *    Only with that change, valgrind (or similar) reliably helps
 *    to detect leaked memory.
 *  - If your code cuts off a feasible solution, but you do not know which component is responsible,
 *    you can use the debugging mechanism (see \ref EXAMPLE_2). Therefore, a given solution is read and it
 *    is checked for every reduction, whether the solution will be pruned globally.
 *
 * @section EXAMPLE_1 How to activate debug messages
 * For example, if we include a <code>\#define SCIP_DEBUG</code> at the top of \ref heur_oneopt.c, recompile SCIP
 * in Debug mode, and run the SCIP interactive shell to solve p0033.mps from the
 * <a href="http://miplib2010.zib.de/miplib3/miplib.html">MIPLIB 3.0</a> , we get some output like:
 *
 * \include debugexamples/example1.txt
 *
 * @section EXAMPLE_2 How to add a debug solution
 *
 * Continuing the example above, we finish the solving process.
 * The optimal solution can now be written to a file:
 * \include debugexamples/example2_1.txt
 *
 * If we afterwards recompile SCIP with the additional compiler flag <code>cmake -DDEBUGSOL=on</code> (<code>make DEBUGSOL=true</code> in the Makefile system),
 * set the parameter <code>misc/debugsol = check/p0033.sol</code>, and run SCIP again it will output:
 * \include debugexamples/example2_2.txt
 * Further debug output would only appear, if the solution was cut off in the solving process.
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page TEST How to run automated tests with SCIP
 *
 *  SCIP comes along with a set of useful tools that allow to perform automated tests. The
 *  following is a step-by-step guide from setting up the test environment for evaluation and
 *  customization of test runs.
 *
 *
 *  @section SETUP Setting up the test environment
 *
 *  At first you should create a file listing all problem instances that should be part of the test.
 *  This file has to be located in the the directory <code>scip/check/testset/</code>
 *  and has to have the file extension <code>.test</code>, e.g., <code>testrun.test</code>,
 *  in order to be found by the <code>scip/check/check.sh</code> script.
 *  \n
 *  All test problems can be listed in the <code>test</code>-file by a relative path,
 *  e.g., <code>../../problems/instance1.lp</code> or absolute path, e.g., <code>/home/problems/instance2.mps</code>
 *  in this file. Only one problem should be listed on every line (since the command <code>cat</code> is used to parse this file).
 *  Note that these problems have to be readable for SCIP in order to solve them.
 *  However, you can use different file formats.
 *
 *  Optionally, you can provide a solution file in the <code>scip/check/testset/</code> directory containing
 *  known information about the feasibility and the best known objective values for the test instances.
 *  SCIP can use these values to verify the results. The file has to have the same basename as the
 *  <code>.test</code>-file, i.e., in our case <code>testrun.solu</code>. One line can only contain
 *  information about one test instance. A line has to start with the type of information given:
 *
 *  - <code>=opt=</code> stating that a problem name with an optimal objective value follows
 *  - <code>=best=</code> stating that a problem name with a best know objective value follows
 *  - <code>=inf=</code> stating that a problem name follows which is infeasible
 *
 *  With these information types you can encode for an instance named <code>instance1.lp</code> the following
 *  information:
 *  - The instance has a known optimal (objective) value of 10.
 *   \code
 *   =opt=  instance1 10
 *   \endcode
 *  - The instance has a best known solution with objective value 15.
 *   \code
 *   =best=  instance1 15
 *   \endcode
 *  - The instance is feasible (but has no objective function or we don't know a solution value)
 *   \code
 *   =feas=  instance1
 *   \endcode
 *  - The instance is infeasible.
 *   \code
 *   =inf=  instance1
 *   \endcode
 *
 *  If you don't know whether the instance is feasible or not (so the status is unknown),
 *  you can omit the instance in the <code>solu</code>-file or write
 *   \code
 *   =unkn=  instance1
 *   \endcode
 *
 * <b>Note that in all lines the file extension of the file name is omitted.</b>
 *  \n
 *  See the files <code>scip/check/testset/short.test</code> and <code>scip/check/testset/short.solu</code>
 *  for an example of a <code>test</code>-file and its corresponding <code>solu</code>-file.
 *
 *
 *
 *  @section STARTING Starting a test run
 *
 *
 *  \code
 *  make TEST=testrun test
 *  \endcode
 *
 *  in the SCIP root directory. Note that <code>testrun</code> is exactly the basename of our
 *  <code>test</code>-file (<code>testrun.test</code>). This will cause SCIP to solve our test instances
 *  one after another and to create various output files (see \ref EVAL).
 *
 *
 *  @section EVAL Evaluating a test run
 *
 *  During computation, SCIP automatically creates the directory <code>scip/check/results/</code>
 *  (if it does not already exist) and stores the following output files there.
 *
 *  \arg <code>*.out</code> - output of <code>stdout</code>
 *  \arg <code>*.err</code> - output of <code>stderr</code>
 *  \arg <code>*.set</code> - copy of the used settings file
 *
 *  \arg <code>*.res</code> - ASCII table containing a summary of the computational results
 *  \arg <code>*.tex</code> - TeX table containing a summary of the computational results
 *  \arg <code>*.pav</code> - <a href="http://www.gamsworld.org/performance/paver/">PAVER</a> output
 *
 *  The last three files in the above list, i.e., the files containing a summary of the computational results,
 *  can also be generated manually. Therefore the user has to call the <code>evalcheck.sh</code> script in the
 *  @c check directory with the corresponding @c out file as argument. For example, this may be useful if the user stopped the
 *  test before it was finished, in which case the last three files will not be automatically generated by SCIP.
 *
 *  The last column of the ASCII summary table contains the solver status. We distinguish the following statuses: (in order of priority)
 *
 *  \arg abort: solver broke before returning solution
 *  \arg fail: solver cut off a known feasible solution (value of the <code>solu</code>-file is beyond the dual bound;
 *  especially if problem is claimed to be solved but solution is not the optimal solution)
 *   <b>or</b> if a final solution check revealed a violation of one of the original constraints.
 *  \arg ok: solver solved problem with the value in solu-file
 *  \arg solved: solver solved problem which has no (optimal) value in solu-file (since we here cannot detect the direction
 *  of optimization, it is possible that a solver claims an optimal solution which contradicts a known feasible solution)
 *  \arg better: solver found solution better than known best solution (or no solution was noted in the <code>solu</code>-file so far)
 *  \arg gaplimit, sollimit: solver reached gaplimit or limit of number of solutions (at present: only in SCIP)
 *  \arg timeout: solver reached any other limit (like time or nodes)
 *  \arg unknown: otherwise
 *
 *  Additionally the <code>evalcheck.sh</code> script can generate a <code>solu</code>-file by calling
 *  \code
 *  ./evalcheck.sh writesolufile=1 NEWSOLUFILE=<solu-file> <out-file>
 *  \endcode
 *  where <code><solu-file></code> denotes the filename of the new file where the solutions shall be
 *  (and <code><out-file></code> denotes the output (<code>.out</code>) files to evaluate).
 *
 *  Another feature can be enabled by calling:
 *  \code
 *  ./evalcheck.sh printsoltimes=1 ...
 *  \endcode
 *  The output has two additional columns containing the solving time until the first and the best solution was found.
 *
 *
 *  @b Note: The @em basename of all these files is the same and has the following structure
 *  which allows us to reconstruct the test run:
 *
 *  \code
 *  check.<test name>.<binary>.<machine name>.<setting name>
 *  \endcode
 *
 *  \arg <<code>test name</code>> indicates the name of the the test file, e.g., <code>testrun</code>
 *  \arg <<code>binary</code>> defines the used binary, e.g., <code>scip-3.2.0.linux.x86_64.gnu.opt.spx</code>
 *  \arg <<code>machine name</code>> tells the name of the machine, e.g., <code>mycomputer</code>
 *  \arg <<code>setting name</code>> denotes the name of the used settings, e.g., <code>default</code>
 *    means the (SCIP) default settings were used
 *
 *  Using the examples out of the previous listing the six file names would have the name:
 *
 *  \code
 *  check.testrun.scip-1.1.0.linux.x86.gnu.opt.spx.mycomputer.default.<out,err,set,res,tex,pav>
 *  \endcode
 *
 *
 *  @section USING Using customized setting files
 *
 *  It is possible to use customized settings files for the test run instead of testing SCIP with default settings.
 *  These have to be placed in the directory <code>scip/settings/</code>.
 *
 *  @b Note: Several common user parameters such as, e.g., the time limit and node limit parameters,
 *           <b>cannot</b> be controlled by the settings file, whose specifications would be overwritten
 *           by optional command line arguments to the <code>make test</code> command, see @ref ADVANCED
 *           for a list of available advanced testing options that have to be specified from the command line.
 *
 *  @b Note: Accessing settings files in subfolders of the @c settings directory is currently not supported.
 *
 *  To run SCIP with a custom settings file, say for example <code>fast.set</code>, we call
 *
 *  \code
 *  make TEST=testrun SETTINGS=fast test
 *  \endcode
 *
 *  in the SCIP root directory. It is possible to enter a list of settings files as a double-quoted,
 *  comma-separated list of settings names as <code>fast</code> above, i.e. <code>SETTINGS="fast,medium,slow"</code>
 *  will invoke the solution process for every instance with the three settings <code>fast.set, medium.set, slow.set</code>
 *  before continuing with the next instance from the <code>.test</code>-file. This may come in handy if the
 *  whole test runs for a longer time and partial results are already available.
 *
 *
 *  @section ADVANCED Advanced options
 *
 *  We can further customize the test run by specifying the following options in the <code>make</code> call:
 *
 *  \arg <code>CONTINUE</code> - continue the test run if it was previously aborted [default: "false"]
 *  \arg <code>DISPFREQ</code> - display frequency of the output [default: 10000]
 *  \arg <code>FEASTOL</code> - LP feasibility tolerance for constraints [default: "default"]
 *  \arg <code>LOCK</code> - should the test run be locked to prevent other machines from performing the same test run [default: "false"]
 *  \arg <code>MAXJOBS=n</code> - run tests on 'n' cores in parallel. Note that several instances are solved in parallel, but
 *                                    only one thread is used per job (parallelization is not that easy) [default: 1]
 *  \arg <code>MEM</code>   -  memory limit in MB [default: 6144]
 *  \arg <code>NODES</code> - node limit [default: 2100000000]
 *  \arg <code>TIME</code>  - time limit for each test instance in seconds [default: 3600]
 *  \arg <code>SETCUTOFF</code> - if set to '1', an optimal solution value (from the <code>.solu</code>-file) is used as objective limit [default: 0]
 *  \arg <code>THREADS</code> - the number of threads used for solving LPs, if the linked LP solver supports multithreading [default: 1]
 *  \arg <code>VALGRIND</code> - run valgrind on the SCIP binary; errors and memory leaks found by valgrind are reported as fails [default: "false"]
 *
 *
 *  @section COMPARE Comparing test runs for different settings
 *
 *  Often test runs are performed on the basis of different settings. In this case, it is useful to
 *  have a performance comparison. For this purpose, we can use the <code>allcmpres.sh</code> script in
 *  the @c check directory.
 *
 *  Suppose, we performed our test run with two different settings, say <code>fast.set</code> and
 *  <code>slow.set</code>. Assuming that all other parameters (including the SCIP binary), were the same,
 *  we may have the following <code>res</code>-files in the directory <code>scip/check/results/</code>
 *
 *  \code
 *  check.testrun.scip-3.2.0.linux.x86_64.gnu.opt.spx.mycomputer.fast.res
 *  check.testrun.scip-3.2.0.linux.x86_64.gnu.opt.spx.mycomputer.slow.res
 *  \endcode
 *
 *  For a comparison of both computations, we simply call
 *
 *  \code
 *  allcmpres.sh results/check.testrun.scip-3.2.0.linux.x86_64.gnu.opt.spx.mycomputer.fast.res \
 *               results/check.testrun.scip-3.2.0.linux.x86_64.gnu.opt.spx.mycomputer.slow.res
 *  \endcode
 *
 *  in the @c check directory. This produces an ASCII table on the console that provide a detailed
 *  performance comparison of both test runs. Note that the first <code>res</code>-file serves as reference
 *  computation. The following list explains the output.
 *  (The term "solver" can be considered as the combination of SCIP with a specific setting file.)
 *
 *  \arg <code>Nodes</code> - Number of processed branch-and-bound nodes.
 *  \arg <code>Time</code>  - Computation time in seconds.
 *  \arg <code>F</code>     - If no feasible solution was found, then '#', empty otherwise.
 *  \arg <code>NodQ</code>  - Equals Nodes(i) / Nodes(0), where 'i' denotes the current solver and '0' stands for the reference solver.
 *  \arg <code>TimQ</code>  - Equals Time(i) / Time(0).
 *  \arg <code>bounds check</code> - Status of the primal and dual bound check.
 *
 *  \arg <code>proc</code> - Number of instances processed.
 *  \arg <code>eval</code> - Number of instances evaluated (bounds check = "ok", i.e., solved to optimality
 *      within the time and memory limit and result is correct). Only these instances are used in the calculation
 *      of the mean values.
 *  \arg <code>fail</code> - Number of instances with bounds check = "fail".
 *  \arg <code>time</code> - Number of instances with timeout.
 *  \arg <code>solv</code> - Number of instances correctly solved within the time limit.
 *  \arg <code>wins</code> - Number of instances on which the solver won (i.e., the
 *      solver was at most 10% slower than the fastest solver OR had the best
 * 	primal bound in case the instance was not solved by any solver within
 *	the time limit).
 *  \arg <code>bett</code>    - Number of instances on which the solver was better than the
 *	reference solver (i.e., more than 10% faster).
 *  \arg <code>wors</code>    - Number of instances on which the solver was worse than the
 *	reference solver (i.e., more than 10% slower).
 *  \arg <code>bobj</code>    - Number of instances on which the solver had a better primal
 *	bound than the reference solver (i.e., a difference larger than 10%).
 *  \arg <code>wobj</code>    - Number of instances on which the solver had a worse primal
 *	bound than the reference solver (i.e., a difference larger than 10%).
 *  \arg <code>feas</code>    - Number of instances for which a feasible solution was found.
 *  \arg <code>gnodes</code>   - Geometric mean of the processed nodes over all evaluated instances.
 *  \arg <code>shnodes</code> - Shifted geometric mean of the processed nodes over all evaluated instances.
 *  \arg <code>gnodesQ</code>  - Equals nodes(i) / nodes(0), where 'i' denotes the current
 *	solver and '0' stands for the reference solver.
 *  \arg <code>shnodesQ</code> - Equals shnodes(i) / shnodes(0).
 *  \arg <code>gtime</code>    - Geometric mean of the computation time over all evaluated instances.
 *  \arg <code>shtime</code>  - Shifted geometric mean of the computation time over all evaluated instances.
 *  \arg <code>gtimeQ</code>   - Equals time(i) / time(0).
 *  \arg <code>shtimeQ</code> - Equals shtime(i) / shtime(0).
 *  \arg <code>score</code>   - N/A
 *
 *  \arg <code>all</code>   - All solvers.
 *  \arg <code>optimal auto settings</code> - Theoretical result for a solver that performed 'best of all' for every instance.
 *  \arg <code>diff</code>  - Solvers with instances that differ from the reference solver in the number of
 *       processed nodes or in the total number of simplex iterations.
 *  \arg <code>equal</code> - Solvers with instances whose number of processed nodes and total number of
 *       simplex iterations is equal to the reference solver (including a 10% tolerance) and where no timeout
 *       occured.
 *  \arg <code>all optimal</code> - Solvers with instances that could be solved to optimality by
 *       <em>all</em> solvers; in particular, no timeout occurred.
 *
 *  Since this large amount of information is not always needed, one can generate a narrower table by calling:
 *  \code
 *  allcmpres.sh short=1 ...
 *  \endcode
 *  where <code>NodQ</code>, <code>TimQ</code> and the additional comparison tables are omitted.
 *
 *  If the <code>res</code>-files were generated with the parameter <code>printsoltimes=1</code>
 *  we can enable the same feature here as well by calling:
 *  \code
 *  allcmpres.sh printsoltimes=1 ...
 *  \endcode
 *  As in the evaluation, the output contains the two additional columns of the solving time until the first and the best solution was found.
 *
 *  @section STATISTICS Statistical tests
 *
 *  The \c allcmpres script also performs two statistical tests for comparing different settings: For deciding whether
 *  more feasible solutions have been found or more instances have been solved to optimality or not, we use a McNemar
 *  test. For comparing the running time and number of nodes, we use a variant of the Wilcoxon signed rank test. A
 *  detailed explanation can be found in the PhD thesis of Timo Berthold (Heuristic algorithms in global MINLP solvers).
 *
 *  @subsection McNemar McNemar test
 *
 *  Assume that we compare two settings \c S1 and \c S2 with respect to the number of instances solved to optimality
 *  within the timelimit. The null hypothesis would be "Both settings lead to an equal number of instances being solved
 *  to optimality", which we would like to disprove. Let \f$n_1\f$ be the number of instances solved by setting \c S1
 *  but not by \c S2, and let \f$n_2\f$ be the number of instances solved by setting \c S2 but not by \c S1.  The
 *  McNemar test statistic is
 *  \f[
 *    \chi^2 = \frac{(n_1 - n_2)^2}{n_1 + n_2}.
 *  \f]
 *  Under the null hypothesis, \f$\chi^2\f$ is chi-squared distributed with one degree of freedom. This allows to compute
 *  a \f$p\f$-value as the probability for obtaining a similar or even more extreme result under the null hypothesis.
 *  More explicitly, \c allcmpres uses the following evaluation:
 *  - \f$0.05 < p\f$: The null hypothesis is accepted (marked by "X").
 *  - \f$0.005 < p \leq 0.05\f$: The null hypothesis might be false (marked by "!").
 *  - \f$0.0005 < p \leq 0.005\f$: The null hypothesis can be false (marked by "!!").
 *  - \f$p \leq 0.0005\f$: The null hypothesis is very likely false (marked by "!!!").
 *
 *  As an example consider the following output:
 *  \code
 *    McNemar (feas)                              x2  0.0000, 0.05 < p           X
 *    McNemar (opt)                               x2  6.0000, p ~ (0.005, 0.05]  !
 *  \endcode
 *  Here, \c x2 represents \f$\chi^2\f$.
 *
 *  In this case, the test with respect to the number of found feasible solutions is irrelevant, since their number is
 *  equal. In particular, the null hypothesis gets accepted (i.e., there is no difference in the settings - this is
 *  marked by "X").
 *
 *  With respect to the number of instances solved to optimality within the timelimit, we have that \f$0.005 < p <=
 *  0.05\f$ (marked by <tt>p ~ (0.005, 0.05)</tt>). Thus, there is some evidence that the null hypothesis is false, i.e., the
 *  settings perform differently; this is marked by "!". In the concrete case, we have 230 instances, all of which are
 *  solved by setting \c S2, but only 224 by setting \c S1.
 *
 *  @subsection Wilcoxon Wilcoxon signed rank test
 *
 *  Assume that we compare two settings \c S1 and \c S2 with respect to their solution times (within the time limit). We
 *  generate a sorted list of the ratios of the run times, where ratios that are (absolutely or relatively) within 1\%
 *  of 1.0 are discarded, and ratios between 0.0 and 0.99 are replaced with their negative inverse in order to
 *  obtain a symmetric distribution for the ratios around the origin.
 *  We then assign ranks 1 to \c N to the remaining \c N data points in nondecreasing
 *  order of their absolute ratio. This yields two groups \c G1
 *  and \c G2 depending on whether the ratios are smaller than -1.0 or larger than 1.0 (\c G1 contains the instances for which
 *  setting \c S1 is faster). Then the sums of the ranks in groups \c G1 and \c G2 are computed, yielding values \c R1
 *  and \c R2, respectively.
 *
 *  The Wilcoxon test statistic is then
 *  \f[
 *     z = \frac{\min(R1, R2) - \frac{N(N+1)}{4}}{\sqrt{\frac{N(N+1)(2N+1)}{24}}},
 *  \f]
 *  which we assume to be (approximately) normally distributed (with zero mean) and allows to compute the probability
 *  \f$p\f$ that one setting is faster than the other. (Note that for \f$N \leq 60\f$, we apply a correction by
 *  subtracting 0.5 from the numerator).
 *
 *  As an example consider the following output:
 *  \code
 *    Wilcoxon (time)                             z  -0.1285, 0.05 <= p          X
 *    Wilcoxon (nodes)                            z -11.9154, p < 0.0005       !!!
 *  \endcode
 *  While the \f$z\f$-value is close to zero for the run time, it is extremely negative regarding the solving nodes. This latter
 *  tendency for the number of nodes is significant on a 0.05 % level, i.e., the probability \f$p\f$ that setting \c S1 uses more
 *  nodes than setting \c S2 is negligible (this null hypothesis is rejected - marked by "!!!").
 *
 *  However, the null hypothesis is not rejected with respect to the run time. In the concrete case, setting \c S1 has a
 *  shifted geometric mean of its run times (over 230 instances) of 248.5, for \c S2 it is 217.6. This makes a ratio of
 *  0.88. Still - the null hypothesis is not rejected.
 *
 *  @section SOLVER Testing and Evaluating using GAMS
 *
 *  Analogously to the target <code>test</code> there is another target to run automated tests with <a href="http://www.gams.com/">gams</a>
 *  \code
 *  make testgams GAMSSOLVER=xyz
 *  \endcode
 *  For this target, the option GAMSSOLVER has to be given to specify the name of a GAMS solver to run, e.g. GAMSSOLVER=SCIP.
 *  Additional advanced options specific to this target are:
 *    GAMS to specify the GAMS executable (default: gams),
 *    GAP to specify a gap limit (default: 0.0),
 *    CLIENTTMPDIR to specify a directory where GAMS should put its scratch files (default: /tmp),
 *    CONVERTSCIP to specify a SCIP which can be used to convert non-gams files into gams format (default: bin/scip, if existing; set to "no" to disable conversion).
 *  The following options are NOT supported (and ignored): DISPFREQ, FEASTOL, LOCK.
 *  A memory limit (MEM option) is only passed as workspace option to GAMS, but not enforced via ulimit (it's up to the solver to regard and obey the limit).
 *
 *  Note: This works only if the referred programs are installed globally on your machine.
 *
 *  The above options like <code>TIME</code> are also available for gams.
 *
 *  After the testrun there should be an <code>.out</code>, an <code>.err</code> and a <code>.res</code> file
 *  with the same basename as described above.
 *
 *  Furthermore you can also use the script <code>allcmpres.sh</code> for comparing results.
 *
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

/**@page COUNTER How to use SCIP to count/enumerate feasible solutions
 *
 * SCIP is capable of computing (count or enumerate) the number of feasible solutions of a given constraint integer
 * program. In case continuous variables are present, the number of feasible solutions for the projection to the
 * integral variables is counted/enumerated. This means, an assignment to the integer variables is counted if the
 * remaining problem (this is the one after fixing the integer variables w.r.t. to this assignment) is feasible.
 *
 * As a first step you have to load or create your problem in the usual way. In case of using the
 * interactive shell, you use the <code>read</code> command:
 *
 * <code>SCIP&gt; read &lt;file name&gt;</code>
 *
 * Afterwards you can count the number of feasible solution with the command <code>count</code>.
 *
 * <code>SCIP&gt; count</code>
 *
 * @note After completing the counting process, SCIP will terminate with status <tt>infeasible</tt>.  This is intended
 * behavior, because SCIP counts solutions by the following internal mechanism.  Each feasible solution that is found is
 * reported as infeasible to the SCIP core. This avoids that SCIP performs reductions based on the primal bound that
 * could cut off suboptimal feasible solutions, which would then be missing in the count.  However, as a result, the
 * SCIP core has not found any feasible solutions during the search and reports status <tt>infeasible</tt>.
 *
 * By default, SCIP only counts the number of solutions but does not store (enumerate) them. If you are interested in
 * that see \ref COLLECTALLFEASEBLES.
 *
 * @note Since SCIP version 2.0.0 you do not have to worry about the impact of dual reductions anymore. These are
 * automatically turned off. The only thing you should switch off are restarts. These restarts can lead to a wrong
 * counting process. We recommend using the counting settings which can be set in the interactive shell as follows:
 *
 * <code>SCIP&gt; set emphasis counter</code>
 *
 * The SCIP callable library provides an interface method SCIPcount() which allows users to count the number of feasible
 * solutions to their problem. The method SCIPsetParamsCountsols(), which is also located in cons_countsols.h, loads the
 * predefined counting settings to ensure a safe count. The complete list of all methods that can be used for counting
 * via the callable library can be found in cons_countsols.h.
 *
 *
 * @section COUNTLIMIT Limit the number of solutions which should be counted
 *
 * It is possible to give a (soft) upper bound on the number solutions that should be counted. If this upper bound is
 * exceeded, SCIP will be stopped. The name of this parameter is <code>constraints/countsols/sollimit</code>. In
 * the interactive shell this parameter can be set as follows:
 *
 * <code>SCIP&gt; set constraints countsols sollimit 1000</code>
 *
 * In case you are using the callable library, this upper bound can be assigned by calling SCIPsetLongintParam() as follows:
 *
 * \code
 * SCIP_CALL( SCIPsetLongintParam( scip, "constraints/countsols/sollimit", 1000) );
 * \endcode
 *
 *
 * The reason why this upper bound is soft comes from the fact that, by default, SCIP uses a technique called unrestricted
 * subtree detection. Using this technique it is possible to detect several solutions at once. Therefore, it can happen
 * that the solution limit is exceeded before SCIP is stopped.
 *
 * @section COLLECTALLFEASEBLES Collect all feasible solutions
 *
 * Per default SCIP only counts all feasible solutions. This means, these solutions are not stored. If you switch the
 * parameter <code>constraints/countsols/collect</code> to TRUE (the default value is FALSE) the detected solutions are
 * stored. Changing this parameter can be done in the interactive shell
 *
 * <code>SCIP&gt; set constraints countsols collect TRUE</code>
 *
 * as well as via the callable library
 *
 * \code
 * SCIP_CALL( SCIPsetBoolParam( scip, "constraints/countsols/collect", TRUE) );
 * \endcode
 *
 * @note The solution which are collected are stored w.r.t. the active variables. These are the variables which got not
 *       removed during presolving.
 *
 * In case you are using the interactive shell you can write all collected solutions to a file as follows
 *
 * <code>SCIP&gt; write allsolutions &lt;file name&gt;</code>
 *
 * In that case the sparse solutions are unrolled and lifted back into the original variable space.
 *
 * The callable library provides a method which gives access to all collected sparse solutions. That is,
 * SCIPgetCountedSparseSolutions(). The sparse solutions you get are defined w.r.t. active variables. To get solutions
 * w.r.t. to the original variables. You have to do two things:
 *
 * -# unroll each sparse solution
 * -# lift each solution into original variable space by extending the solution by those variable which got removed
 *    during presolving
 *
 * The get the variables which got removed during presolving, you can use the methods SCIPgetFixedVars() and
 * SCIPgetNFixedVars(). The method SCIPgetProbvarLinearSum() transforms given variables, scalars and constant to the
 * corresponding active variables, scalars and constant. Using this method for a single variable gives a representation
 * for that variable w.r.t. the active variables which can be used to compute the value for the considered solution (which
 * is defined w.r.t. active variables).
 *
 * For that complete procedure you can also check the source code of
 * \ref SCIP_DECL_DIALOGEXEC(SCIPdialogExecWriteAllsolutions) "SCIPdialogExecWriteAllsolutions()" cons_countsols.c which
 * does exactly that.
 *
 *
 * @section COUNTOPTIMAL Count number of optimal solutions
 *
 * If you are interested in counting the number of optimal solutions, this can be done with SCIP using the
 * <code>count</code> command by applying the following steps:
 *
 *  -# Solve the original problem to optimality and let \f$c^*\f$ be the optimal value
 *  -# Added the objective function as constraint with left and right hand side equal to \f$c^*\f$
 *  -# load the adjusted problem into SCIP
 *  -# use the predefined counting settings
 *  -# start counting the number of feasible solutions
 *
 * If you do this, SCIP will collect all optimal solutions of the original problem.
 *
 */

/**@page LICENSE License
 *
 * \verbinclude COPYING
 */

/**@page FAQ Frequently Asked Questions (FAQ)
 * \htmlinclude faq/faq.inc
 */


/**@page PARAMETERS List of all SCIP parameters
 *
 * This page list all parameters of the current SCIP version. This list can
 * easily be generated by SCIP via the interactive shell using the following command:
 *
 * <code>SCIP&gt; set save &lt;file name&gt;</code>
 *
 * or via the function call:
 *
 * <code>SCIP_CALL( SCIPwriteParams(scip, &lt;file name&gt;, TRUE, FALSE) );</code>
 *
 * \verbinclude parameters.set
 */

/**@page INTERFACES Interfaces
  *
  * There are several ways of accessing the \SCIP Optimization Suite from other software packages or programming
  * platforms.
  *
  *
  * @section FILEFORMATS File formats
  *
  *  The easiest way to load a problem into SCIP is via an input file, given in a format that SCIP can parse directly,
  *  see \ref SHELL "the tutorial on how to use the interactive shell".
  *  \SCIP is capable of reading more than ten different file formats, including formats for nonlinear
  *  problems and constraint programs. This gives researchers from different communities an easy, first access to the
  *  \SCIP Optimization Suite. See also the \ref AVAILABLEFORMATS "list of readable file formats".
  *
  * @section MODELLING Modeling languages and Matlab interface
  *
  * A natural way of formulating an optimization problem is to use a modeling language. Besides ZIMPL there are several
  * other modeling tools with a direct interface to \SCIP. These include <a href="http://dynadec.com/">Comet</a>, a
  * modeling language for constraint programming, <a href="http://www.ampl.com/">AMPL</a> and <a
  * href="http://www.gams.com/">GAMS</a>, which are well-suited for modeling mixed-integer linear and nonlinear
  * optimization problems, and <a href="https://projects.coin-or.org/Cmpl">CMPL</a> for mixed-integer linear problems.
  * The AMPL and ZIMPL interfaces are included in the \SCIP distribution, the GAMS interface is available <a
  * href="https://github.com/coin-or/GAMSlinks">here</a>.
  *
  * The <a href="http://www.i2c2.aut.ac.nz/Wiki/OPTI/index.php">OPTI project</a> by Jonathan Currie provides an external
  * MATLAB interface for the \SCIP Optimization Suite. Furthermore,
  * <a href="http://users.isy.liu.se/johanl/yalmip/pmwiki.php?n=Main.HomePage">YALMIP</a> by Johan L&ouml;fberg provides a
  * free modeling language.
  *
  *
  * @section CPLUSPLUS C++ wrapper classes
  *
  * Since \SCIP is written in C, its callable library can be directly accessed from C++. If a user wants to program own
  * plugins in C++, there are wrapper classes for all different types of plugins available in the <code>src/objscip</code>
  * directory of the \SCIP standard distribution. SCIP provides several examples that were written in C++, see
  * \ref EXAMPLES "Examples" and select an example written in C++.
  *
  *
  * @section OTHER Interfaces for other programming languages
  *
  * Interfaces for other programming languages are developed and maintained independently from the SCIP Optimization Suite
  * on <a href="https://github.com/scipopt">GitHub</a> in order to provide extensions and patches faster
  * and to collaborate on them more easily. Besides the popular interfaces for Python and Java, there is also an interface
  * for Julia available. Contributions to these projects are very welcome.
  *
  * There are also several third-party python interfaces to the \SCIP Optimization Suite, e.g., <a
  * href="http://numberjack.ucc.ie/">NUMBERJACK</a> and <a
  * href="http://code.google.com/p/python-zibopt/">python-zibopt</a>. <a href="http://numberjack.ucc.ie/">NUMBERJACK</a>
  * is a constraint programming platform implemented in python. It supports a variety of different solvers, one of them
  * being the \SCIP Optimization Suite. <a href="http://code.google.com/p/python-zibopt/">python-zibopt</a> was developed
  * by Ryan J. O'Neil and is a python extension of the \SCIP Optimization Suite. <a
  * href="http://picos.zib.de/">PICOS</a> is a python interface for conic optimization, provided by Guillaume Sagnol.
  *
  *
  */

 /**@defgroup PUBLICAPI Public API of SCIP
  * @brief methods and headers of the public C-API of \SCIP.  Please refer to \ref DOC "" for information how to use the reference manual.
  *
  * \PUBLICAPIDESCRIPTION
  *
  *
  */

/**@defgroup PUBLICCOREAPI Core API
 * @ingroup PUBLICAPI
 * @brief methods and headers of the plugin-independent C-API provided by the \SCIP header file scip.h.
 *
 * This module comprises methods provided by the header file scip.h. Including this header into a user-written extension
 * suffices to have all plugin-independent functionality of \SCIP available. Plugin-independent
 * user functionality includes the
 *
 * - creation of problems that \SCIP should solve
 * - fine-grained access to initiate the solving process of \SCIP
 * - access to all sorts of solving process statistics
 * - commonly used data structures and algorithms
 * - the management of plugins
 * - ...
 *
 * In order facilitate the navigation through the core API of \SCIP, it is structured into different modules.
 */

/**@defgroup TYPEDEFINITIONS Type Definitions
 * @ingroup PUBLICCOREAPI
 * @brief type definitions and callback declarations
 *
 * This page lists headers which contain type definitions of callback methods.
 *
 * All headers below include the descriptions of callback methods of
 * certain plugins. For more detail see the corresponding header.
 */

/**@defgroup PublicProblemMethods Problem Creation
 * @ingroup PUBLICCOREAPI
 * @brief methods to create a problem that \SCIP should solve
 *
 * This module summarizes the main methods needed to create a problem for \SCIP, and access its most important members:
 * - Declaring, adding, accessing, and changing variables of the problem
 * - Declaring, adding, accessing, and changing constraints of the problem
 * - Creating, adding, accessing, changing, and checking of solutions to the problem
 *
 * @note These core methods are not sufficient to create constraints of a certain type that is provided by the default plugins of \SCIP.
 *  An example would be the creation of a linear constraint for which the methods provided by the
 *  \ref cons_linear.h "linear constraint handler" must be used. Such methods are provided by the default plugins of \SCIP
 *  and can be found in the \ref PUBLICPLUGINAPI.
 */

/**@defgroup GeneralSCIPMethods SCIP
 * @ingroup PublicProblemMethods
 * @brief methods to manipulate a SCIP object
 */

/**@defgroup GlobalProblemMethods Global Problem
 * @ingroup PublicProblemMethods
 * @brief methods to create, read and modify a global problem together with its callbacks
 */

/**@defgroup PublicVariableMethods Problem Variables
 * @ingroup PublicProblemMethods
 * @brief public methods for problem variables
 */

/**@defgroup PublicConstraintMethods Problem Constraints
 * @ingroup PublicProblemMethods
 * @brief Public methods for constraints
 */

/**@defgroup PublicSolutionMethods Primal Solution
 * @ingroup PublicProblemMethods
 * @brief  methods to create and change primal solutions of \SCIP
 */

/**@defgroup CopyMethods Problem Copies
 * @ingroup PublicProblemMethods
 * @brief methods to copy problems between a source and a target \SCIP
 */


 /**@defgroup PublicSolveMethods Solving Process
 * @ingroup PUBLICCOREAPI
 * @brief  methods to control the solving process of \SCIP
 *
 * This large group of methods and modules comprises the solving process related API of \SCIP. This includes
 *
 * -# changing parameters to control the solver behavior
 * -# access search tree related information
 * -# access relaxation information
 * -# access various solving process statistics
 * -# solve auxiliary relaxations and subproblems using LP/NLP diving or probing mode
 */

/**@defgroup PublicSolvingStatsMethods Solving Statistics
 * @ingroup PublicSolveMethods
 * @brief methods to query statistics about the solving process
 */

/**@defgroup ParameterMethods Parameter
 * @ingroup PublicSolveMethods
 * @brief methods to create, query, and print user parameters
 */

/**@defgroup PublicEventMethods Event Handling
 * @ingroup PublicSolveMethods
 * @brief  methods to create, catch, process, and drop events during the solving process of \SCIP
 *
 * Events can only be caught during the operation on the transformed problem.
 * Events on variables can only be caught for transformed variables.
 * If you want to catch an event for an original variable, you have to get the corresponding transformed variable
 * with a call to SCIPgetTransformedVar() and catch the event on the transformed variable.
 */

/**@defgroup PublicLPMethods LP Relaxation
 * @ingroup PublicSolveMethods
 * @brief methods to build and access LP relaxation information
 * @see methods to interact with \ref PublicColumnMethods "LP columns" and \ref PublicRowMethods "LP rows"
 */

/**@defgroup PublicColumnMethods LP Column
 * @ingroup PublicLPMethods
 * @brief public methods for LP columns
 */

/**@defgroup PublicRowMethods LP Row
 * @ingroup PublicLPMethods
 * @brief public methods for LP rows
 */

/**@defgroup PublicCutMethods Cuts and Cutpools
 * @ingroup PublicLPMethods
 * @brief common methods used to manipulate, generate, and strengthen cuts and to organize the cutpool
 */

/**@defgroup PublicLPDivingMethods LP Diving
 * @ingroup PublicLPMethods
 * @brief methods to initiate and conduct LP diving
 */

/**@defgroup PublicConflictMethods Conflict Analysis
 * @ingroup PublicSolveMethods
 * @brief public methods for conflict analysis
 */

/**@defgroup PublicNLPMethods NLP Relaxation
 * @ingroup PublicSolveMethods
 * @brief methods for the nonlinear relaxation
 */

/**@defgroup PublicExpressionTreeMethods Expression (Tree)
 * @ingroup PublicNLPMethods
 * @brief methods for expressions and expression trees
 */

/**@defgroup PublicNLRowMethods Nonlinear Rows
 * @ingroup PublicNLPMethods
 * @brief methods for the creation and interaction with rows of the NLP relaxation
 */

/**@defgroup PublicNLPDiveMethods NLP Diving
 * @ingroup PublicNLPMethods
 * @brief methods to initiate and conduct NLP Diving
 */

/**@defgroup PublicBranchingMethods Branching
 * @ingroup PublicSolveMethods
 * @brief methods for branching on LP solutions, relaxation solutions, and pseudo solutions
 *
 * @see \ref PublicVariableMethods "Public Variable methods" contains some typical variable branching score functions
 */

/**@defgroup LocalSubproblemMethods Local Subproblem
 * @ingroup PublicSolveMethods
 * @brief methods to query information about or strengthen the problem at the current local search node
 */

/**@defgroup PublicTreeMethods Search Tree
 * @ingroup PublicSolveMethods
 * @brief  methods to query search tree related information
 * @see \ref PublicNodeMethods "Public methods for nodes"
 */

/**@defgroup PublicNodeMethods Nodes
 * @ingroup PublicTreeMethods
 * @brief  methods for nodes of the search tree of \SCIP
 */

/**@defgroup PublicProbingMethods Probing
 * @ingroup PublicSolveMethods
 * @brief methods to initiate and control the probing mode of \SCIP
 */

/**@defgroup PublicReoptimizationMethods Reoptimization
 * @ingroup PublicSolveMethods
 * @brief methods for reoptimization related tasks
 */

/** @defgroup DataStructures Data Structures
 *  @ingroup PUBLICCOREAPI
 *  @brief commonly used data structures
 *
 *  Below you find a list of available data structures
 */

/** @defgroup DisjointSet Disjoint Set (Union Find)
 *  @ingroup DataStructures
 *  @brief weighted disjoint set (union find) data structure with path compression
 *
 *  Weighted Disjoint Set is a data structure to quickly update and query connectedness information
 *  between nodes of a graph. Disjoint Set is also known as Union Find.
 */

/**@defgroup PublicDynamicArrayMethods Dynamic Arrays
 * @ingroup DataStructures
 * @brief methods for the creation and access of dynamic arrays
 */

/**@defgroup DirectedGraph Directed Graph
 * @ingroup DataStructures
 * @brief graph structure with common algorithms for directed and undirected graphs
 */

/**@defgroup DecompMethods Decomposition data structure
 * @ingroup DataStructures
 * @brief methods for creating and accessing user decompositions
 */

/**@defgroup MiscellaneousMethods Miscellaneous Methods
 * @ingroup PUBLICCOREAPI
 * @brief commonly used methods from different categories
 *
 * Below you find a list of miscellaneous methods grouped by different categories
 */

/**@defgroup PublicValidationMethods Validation
 * @ingroup PUBLICCOREAPI
 * @brief  methods for validating the correctness of a solving process
 */

/**@defgroup PublicMemoryMethods Memory Management
 * @ingroup MiscellaneousMethods
 * @brief  methods and macros to use the \SCIP memory management
 *
 * @see \ref MEMORY  "Using the memory functions of SCIP" for more information
 */

/**@defgroup PublicNonlinearMethods Nonlinear Data
 * @ingroup MiscellaneousMethods
 * @brief methods for nonlinear data
 */

/**@defgroup PublicTimingMethods Timing
 * @ingroup MiscellaneousMethods
 * @brief  methods for timing
 */

/**@defgroup PublicBanditMethods Bandit Algorithms
 * @ingroup MiscellaneousMethods
 * @brief  methods for bandit algorithms
 */



/**@defgroup DebugSolutionMethods Debug Solution
 * @ingroup PUBLICCOREAPI
 * @brief methods to control the SCIP debug solution mechanism, see also \ref DEBUG
 */

/**@defgroup MessageOutputMethods Messaging
 * @ingroup PUBLICCOREAPI
 * @brief message output methods
 */

/**@defgroup PluginManagementMethods Methods for managing plugins
 * @ingroup PUBLICCOREAPI
 * @brief Methods for the inclusion and management of SCIP plugins and callback functions
 *
 */

/**@defgroup PublicBendersMethods Benders' decomposition
 * @ingroup PluginManagementMethods
 * @brief  methods for Benders' decomposition plugins
 */

/**@defgroup PublicBenderscutsMethods Benders' decomposition cuts
 * @ingroup PluginManagementMethods
 * @brief  methods for Benders' decomposition cuts plugins
 */

/**@defgroup PublicBranchRuleMethods Branching Rules
 * @ingroup PluginManagementMethods
 * @brief  methods for branching rule plugins
 */

/**@defgroup PublicCompressionMethods Tree Compression
 * @ingroup PluginManagementMethods
 * @brief  public methods for tree compressions
 */

/**@defgroup PublicConflicthdlrMethods Conflict Analysis
 * @ingroup PluginManagementMethods
 * @brief public methods for conflict handlers
 */

/**@defgroup PublicConshdlrMethods Constraint handlers
 * @ingroup PluginManagementMethods
 * @brief methods for constraint handlers
 */

/**@defgroup PublicCutSelectorMethods Cut Selector
 * @ingroup PluginManagementMethods
 * @brief methods for cut selectors
 */

/**@defgroup PublicDialogMethods Dialogs
 * @ingroup PluginManagementMethods
 * @brief public methods for user interface dialogs
 */

/**@defgroup PublicDisplayMethods Displays
 * @ingroup PluginManagementMethods
 * @brief  methods for the inclusion and access of display plugins
 */

/**@defgroup PublicEventHandlerMethods Event Handler
 * @ingroup PluginManagementMethods
 * @brief  methods for event handlers
 */

/**@defgroup PublicHeuristicMethods Primal Heuristics
 * @ingroup PluginManagementMethods
 * @brief  methods for primal heuristic plugins
 */

/**@defgroup PublicDivesetMethods Dive sets
 * @ingroup PublicSpecialHeuristicMethods
 * @brief methods for dive sets to control the generic diving algorithm
 */

/**@defgroup PublicNodeSelectorMethods Node Selector
 * @ingroup PluginManagementMethods
 * @brief  methods for node selector plugin management
 */

/**@defgroup PublicPresolverMethods Presolver
 * @ingroup PluginManagementMethods
 * @brief methods for presolver plugins
 */

/**@defgroup PublicPricerMethods Pricer
 * @ingroup PluginManagementMethods
 * @brief  methods to include and access pricer plugins of \SCIP
 */

/**@defgroup PublicPropagatorMethods Propagators
 * @ingroup PluginManagementMethods
 * @brief  methods for propagator plugins
 */

/**@defgroup PublicReaderMethods Reader
 * @ingroup PluginManagementMethods
 * @brief methods for the inclusion and access to reader plugins of \SCIP
 */

/**@defgroup PublicRelaxatorMethods Relaxation Handlers
 * @ingroup PluginManagementMethods
 * @brief public methods for relaxation handlers
 */

/**@defgroup PublicSeparatorMethods Separators
 * @ingroup PluginManagementMethods
 * @brief methods for separator plugins
 */

/**@defgroup PublicTableMethods Tables
 * @ingroup PluginManagementMethods
 * @brief  methods for the inclusion and access of statistics tables
 */

/**@defgroup PublicConcsolverTypeMethods Concurrent Solver Types
 * @ingroup PluginManagementMethods
 * @brief methods for concurrent solver type plugins
 */

/**@defgroup PublicNLPInterfaceMethods NLP interfaces
 * @ingroup PluginManagementMethods
 * @brief  methods for the management of NLP interfaces
 */

/**@defgroup PublicExternalCodeMethods External Codes
 * @ingroup PluginManagementMethods
 * @brief methods to access information about external codes used by \SCIP
 */

/**@defgroup PublicParallelMethods Parallel Interface
 * @ingroup PUBLICCOREAPI
 * @brief methods to construct the parallel interface of \SCIP
 */

 /**@defgroup PUBLICPLUGINAPI Plugin API of SCIP
  * @ingroup PUBLICAPI
  * @brief core API extensions provided by the default plugins of \SCIP, includable via scipdefplugins.h.
  *
  * All default plugins of \SCIP, especially the default \ref CONSHDLRS "constraint handlers", provide
  * valuable extensions to the \ref PUBLICCOREAPI "core API" of \SCIP. These methods are made available
  * by including scipdefplugins.h to user-written extensions.
  *
  * For a better overview, this page lists all default plugin headers structured into modules based on their individual
  * topic.
  *
  * All of the modules listed below provide functions that are allowed to be used by user-written extensions of \SCIP.
  */
 /**@defgroup INTERNALAPI Internal API of SCIP
  * @brief internal API methods that should only be used by the core of \SCIP
  *
  * This page lists the header files of internal API methods. In contrast to the public API, these internal methods
  * should not be used by user plugins and extensions of SCIP. Please consult
  * \ref PUBLICCOREAPI "the Core API" and \ref PUBLICPLUGINAPI "Plugin API" for the complete API available to user plugins.
  *
  */

/**@defgroup BENDERS Benders' decomposition implementations
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default Benders' decomposition implementations of \SCIP
 *
 * A detailed description what a Benders' decomposition implementation does and how to add a Benders' decomposition
 * implementations to SCIP can be found
 * \ref BENDER "here".
 */

/**@defgroup BendersIncludes Inclusion methods
 * @ingroup BENDERS
 * @brief methods to include specific Benders' decomposition implementations into \SCIP
 *
 * This module contains methods to include specific Benders' decomposition implementations into \SCIP.
 *
 * @note All default plugins can be included at once (including all Benders' decomposition implementations) using
 * SCIPincludeDefaultPlugins()
 *
 */

/**@defgroup BENDERSCUTS Benders' decomposition cut method
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default Benders' decomposition cut method of \SCIP
 *
 * A detailed description what a Benders' decomposition cut method does and how to add a Benders' decomposition
 * cut method to SCIP can be found
 * \ref BENDERSCUT "here".
 */

/**@defgroup BenderscutIncludes Inclusion methods
 * @ingroup BENDERSCUTS
 * @brief methods to include specific Benders' decomposition cut methods into \SCIP
 *
 * This module contains methods to include specific Benders' decomposition cut methods into \SCIP.
 *
 * @note The Benders' decomposition cut methods are linked to each Benders' decomposition implementation. Thus, the
 * default Benders' decomposition implementations automatically include the necessary Benders' decomposition cut
 * methods. For custom Benders' decomposition implementations, you can call SCIPincludeDefaultBendersCuts() in the
 * SCIPincludeBendersMybenders() include function.
 *
 */

/**@defgroup BRANCHINGRULES Branching Rules
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default branching rules of \SCIP
 *
 * A detailed description what a branching rule does and how to add a branching rule to SCIP can be found
 * \ref BRANCH "here".
 */

/**@defgroup BranchingRuleIncludes Inclusion methods
 * @ingroup BRANCHINGRULES
 * @brief methods to include specific branching rules into \SCIP
 *
 * This module contains methods to include specific branching rules into \SCIP.
 *
 * @note All default plugins can be included at once (including all branching rules) using SCIPincludeDefaultPlugins()
 *
 */

/**@defgroup CONSHDLRS  Constraint Handlers
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default constraint handlers of \SCIP
 *
 * A detailed description what a constraint handler does and how to add a constraint handler to SCIP can be found
 * \ref CONS "here".
 */

/**@defgroup ConshdlrIncludes Inclusion methods
 * @ingroup CONSHDLRS
 * @brief methods to include specific constraint handlers into \SCIP
 *
 * This module contains methods to include specific constraint handlers into \SCIP.
 *
 * @note All default plugins can be included at once (including all default constraint handlers) using SCIPincludeDefaultPlugins()
 *
 */

/**@defgroup CUTSELECTORS Cut Selectors
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default cut selectors of \SCIP
 *
 * A detailed description what a cut selector does and how to add a cut selector to SCIP can be found
 * \ref CUTSEL "here".
 */

/**@defgroup CutSelectorIncludes Inclusion methods
 * @ingroup CUTSELECTORS
 * @brief methods to include specific cut selectors into \SCIP
 *
 * This module contains methods to include specific cut selectors into \SCIP.
 *
 * @note All default plugins can be included at once (including all default cut selectors) using SCIPincludeDefaultPlugins()
 *
 */

/**@defgroup DIALOGS Dialogs
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default dialogs of \SCIP
 *
 * A detailed description what a dialog does and how to add a dialog to SCIP can be found
 * \ref DIALOG "here".
 */

/**@defgroup DialogIncludes Inclusion methods
 * @ingroup DIALOGS
 * @brief methods to include specific dialogs into \SCIP
 *
 * This module contains methods to include specific dialogs into \SCIP.
 *
 * @note All default plugins can be included at once (including all default dialogs) using SCIPincludeDefaultPlugins()
 *
 */

/**@defgroup DISPLAYS Displays
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default displays (output columns)  of \SCIP
 *
 * A detailed description what a display does and how to add a display to SCIP can be found
 * \ref DISP "here".
 *
 */

/**@defgroup DisplayIncludes Inclusion methods
 * @ingroup DISPLAYS
 * @brief methods to include specific displays into \SCIP
 *
 * This module contains methods to include specific displays into \SCIP.
 *
 * @note All default plugins can be included at once (including all default displays) using SCIPincludeDefaultPlugins()
 *
 */

/**@defgroup FILEREADERS File Readers
 * @ingroup PUBLICPLUGINAPI
 * @brief This page contains a list of all file readers which are currently available.
 *
 * @section AVAILABLEFORMATS List of readable file formats
 *
 * The \ref SHELL "interactive shell" and the callable library are capable of reading/parsing several different file
 * formats.
 *
 * <table>
 * <tr><td>\ref reader_cip.h "CIP format"</td> <td>for SCIP's constraint integer programming format</td></tr>
 * <tr><td>\ref reader_cnf.h "CNF format"</td> <td>DIMACS CNF (conjunctive normal form) file format used for example for SAT problems</td></tr>
 * <tr><td>\ref reader_diff.h "DIFF format"</td> <td>for reading a new objective function for mixed-integer programs</td></tr>
 * <tr><td>\ref reader_fzn.h "FZN format"</td> <td>FlatZinc is a low-level solver input language that is the target language for MiniZinc</td></tr>
 * <tr><td>\ref reader_lp.h  "LP format"</td>  <td>for mixed-integer (quadratically constrained quadratic) programs (CPLEX)</td></tr>
 * <tr><td>\ref reader_mps.h "MPS format"</td> <td>for mixed-integer (quadratically constrained quadratic) programs</td></tr>
 * <tr><td>\ref reader_opb.h "OPB format"</td> <td>for pseudo-Boolean optimization instances</td></tr>
 * <tr><td>\ref reader_osil.h "OSiL format"</td> <td>for mixed-integer nonlinear programs</td></tr>
 * <tr><td>\ref reader_pip.h "PIP format"</td> <td>for <a href="http://polip.zib.de/pipformat.php">mixed-integer polynomial programming problems</a></td></tr>
 * <tr><td>\ref reader_sol.h "SOL format"</td> <td>for solutions; XML-format (read-only) or raw SCIP format</td></tr>
 * <tr><td>\ref reader_wbo.h "WBO format"</td> <td>for weighted pseudo-Boolean optimization instances</td></tr>
 * <tr><td>\ref reader_zpl.h "ZPL format"</td> <td>for <a href="http://zimpl.zib.de">ZIMPL</a> models, i.e., mixed-integer linear and nonlinear
 *                                                 programming problems [read only]</td></tr>
 * </table>
 *
 * @section ADDREADER How to add a file reader
 *
 * A detailed description what a file reader does and how to add a file reader to SCIP can be found
 * \ref READER "here".
 *
 */

/**@defgroup FileReaderIncludes Inclusion methods
 * @ingroup FILEREADERS
 * @brief methods to include specific file readers into \SCIP
 *
 * This module contains methods to include specific file readers into \SCIP.
 *
 * @note All default plugins can be included at once (including all default file readers) using SCIPincludeDefaultPlugins()
 *
 */

/**@defgroup PARALLEL Parallel interface methods
 * @ingroup INTERNALAPI
 * @brief headers and methods for the parallel interface of \SCIP
 *
 */

/**@defgroup PublicSymmetryMethods Symmetry
 * @ingroup INTERNALAPI
 * @brief methods for symmetry handling
 */

/**@defgroup EXPRINTS Expression Interpreter
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default expression interpreters of \SCIP
 *
 * A detailed description what a expression interpreter does and how to add a expression interpreter to SCIP can be found
 * \ref EXPRINT "here".
 */

/**@defgroup ExprintIncludes Inclusion methods
 * @ingroup EXPRINTS
 * @brief methods to include specific expression interpreters into \SCIP
 *
 * This module contains methods to include specific expression interpreters into \SCIP.
 *
 * @note All default plugins can be included at once (including all default expression interpreters) using SCIPincludeDefaultPlugins()
 *
 */

/**@defgroup FileReaderIncludes Inclusion methods
 * @ingroup FILEREADERS
 * @brief methods to include specific file readers into \SCIP
 *
 * This module contains methods to include specific file readers into \SCIP.
 *
 * @note All default plugins can be included at once (including all default file readers) using SCIPincludeDefaultPlugins()
 *
 */

/**@defgroup LPIS LP Solver Interface
 * @ingroup PUBLICPLUGINLPI
 * @brief methods and files provided by the LP solver interface of \SCIP
 *
 * \SCIP uses external tools to solve LP relaxations. The communication
 * is realized through an LP interface.
 *
 * This page lists public interface methods that every LP interface provides.
 * Find the concrete implementation for your LP solver
 * under "src/lpi/".
 *
 * @see \ref LPI for a list of available LP solver interfaces
 */

/**@defgroup NODESELECTORS Node Selectors
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default node selectors of \SCIP
 *
 * A detailed description what a node selector does and how to add a node selector to SCIP can be found
 * \ref NODESEL "here".
 */

/**@defgroup NodeSelectorIncludes Inclusion methods
 * @ingroup NODESELECTORS
 * @brief methods to include specific node selectors into \SCIP
 *
 * This module contains methods to include specific node selectors into \SCIP.
 *
 * @note All default plugins can be included at once (including all default node selectors) using SCIPincludeDefaultPlugins()
 *
 */

/**@defgroup NLPIS NLP Solver Interfaces
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default NLP solver interfaces of \SCIP
 *
 * A detailed description what a NLP solver interface does and how to add a NLP solver interface to SCIP can be found
 * \ref NLPI "here".
 */

/**@defgroup NLPIIncludes Inclusion methods
 * @ingroup NLPIS
 * @brief methods to include specific NLP solver interfaces into \SCIP
 *
 * This module contains methods to include specific NLP solver interfaces into \SCIP.
 *
 * @note All default plugins can be included at once (including all default NLP solver interfaces) using SCIPincludeDefaultPlugins()
 *
 */

/**@defgroup PRESOLVERS Presolvers
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default presolvers of \SCIP
 *
 * A detailed description what a presolver does and how to add a presolver to SCIP can be found
 * \ref PRESOL "here".
 */

/**@defgroup PresolverIncludes Inclusion methods
 * @ingroup PRESOLVERS
 * @brief methods to include specific presolvers into \SCIP
 *
 * This module contains methods to include specific presolvers into \SCIP.
 *
 * @note All default plugins can be included at once (including all default presolvers) using SCIPincludeDefaultPlugins()
 *
 */

/**@defgroup PRICERS Pricers
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default pricers of \SCIP
 *
 * Per default there exist no variable pricer. A detailed description what a variable pricer does and how to add a
 * variable pricer to SCIP can be found \ref PRICER "here".
 */

/**@defgroup PricerIncludes Inclusion methods
* @ingroup PUBLICPLUGINAPI
* @brief methods to include specific pricers into \SCIP
*
* This module contains methods to include specific pricers into \SCIP.
*
* @note All default plugins can be included at once using SCIPincludeDefaultPlugins(). There exists no pricer per default.
*       In order to see examples of variable pricers, please consult the \ref EXAMPLES "Coding Examples" of \SCIP.
*
*/

/**@defgroup PRIMALHEURISTICS Primal Heuristics
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default primal heuristics of \SCIP
 *
 * A detailed description what a primal heuristic does and how to add a primal heuristic to SCIP can be found
 * \ref HEUR "here".
 */

/**@defgroup PrimalHeuristicIncludes Inclusion methods
 * @ingroup PRIMALHEURISTICS
 * @brief methods to include specific primal heuristics into \SCIP
 *
 * This module contains methods to include specific primal heuristics into \SCIP.
 *
 * @note All default plugins can be included at once (including all default primal heuristics) using SCIPincludeDefaultPlugins()
 *
 */

/**@defgroup PROPAGATORS Propagators
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default propagators of \SCIP
 *
 * A detailed description what a propagator does and how to add a propagator to SCIP can be found
 * \ref PROP "here".
 */

/**@defgroup PropagatorIncludes Inclusion methods
 * @ingroup PROPAGATORS
 * @brief methods to include specific propagators into \SCIP
 *
 * This module contains methods to include specific propagators into \SCIP.
 *
 * @note All default plugins can be included at once (including all default propagators) using SCIPincludeDefaultPlugins()
 *
 */

/**@defgroup RELAXATORS Relaxation Handlers
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default relaxation handlers of \SCIP
 *
 * A detailed description what a relaxation handler does and how to add a relaxation handler to SCIP can be found
 * \ref RELAX "here". Note that the linear programming relaxation is not implemented via the relaxation handler plugin.
 * Per default no relaxation handler exists in SCIP. However, there are two relaxation handlers in the
 * \ref RELAXATOR_MAIN "Relaxator example".
 */

/**@defgroup SEPARATORS Separators
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default separators  of \SCIP
 *
 * A detailed description what a separator does and how to add a separator to SCIP can be found
 * \ref SEPA "here".
 */

/**@defgroup SeparatorIncludes Inclusion methods
 * @ingroup SEPARATORS
 * @brief methods to include specific separators into \SCIP
 *
 * This module contains methods to include specific separators into \SCIP.
 *
 * @note All default plugins can be included at once (including all default separators) using SCIPincludeDefaultPlugins()
 *
 */

/**@defgroup TABLES Tables
 * @ingroup PUBLICPLUGINAPI
 * @brief methods and files provided by the default statistics tables of \SCIP
 *
 * A detailed description what a table does and how to add a table to SCIP can be found
 * \ref TABLE "here".
 *
 */

/**@defgroup TableIncludes Inclusion methods
 * @ingroup TABLES
 * @brief methods to include specific tables into \SCIP
 *
 * This module contains methods to include specific statistics tables into \SCIP.
 *
 * @note All default plugins can be included at once (including all default statisticstables) using SCIPincludeDefaultPlugins()
 *
 */

/**@defgroup PublicToleranceMethods Computations With Tolerances
 * @ingroup NumericalMethods
 * @brief  methods used by the majority of operations involving floating-point computations in \SCIP
 *
 * - query the numerical tolerances of \SCIP, as well as special values such as infinity.
 * - change tolerances inside relaxations
 * - epsilon-comparison methods for floating point numbers
 */

/** @defgroup CFILES Implementation files (.c files)
 *  @brief implementation files (.c files) of the SCIP core and the default plugins
 *
 *  The core provides the functionality for creating problems, variables, and general constraints.
 *  The default plugins of SCIP provide a mix of public API function and private, static function and callback declarations.
 */

/** @defgroup DEFPLUGINS SCIP Default Plugins
 *  @ingroup CFILES
 *  @brief implementation files (.c files) of the SCIP default plugins
 *
 *  The SCIP default plugins provide a mix of public API function and private, static function and callback declarations.
 */

/** @defgroup OTHER_CFILES Other implementation files of SCIP
 *  @ingroup CFILES
 *  @brief other implementation files of SCIP
 *
 *  Relevant core and other functionality of SCIP.
 */
