/***************************************************************************
 *   copyright       : (C) 2003-2007 by Pascal Brachet                     *
 *   http://www.xm1math.net/texmaker/                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SYMBOLLISTWIDGET_H
#define SYMBOLLISTWIDGET_H

#include "mostQtHeaders.h"

typedef QString codelist[412];
const codelist code= {
	"\\pm",//"img1.png"
	"\\cap",//"img2.png"
	"\\diamond",//"img3.png"
	"\\oplus",//"img4.png"
	"\\mp",//"img5.png"
	"\\cup",//"img6.png"
	"\\bigtriangleup",//"img7.png"
	"\\ominus",//"img8.png"
	"\\times",//"img9.png"
	"\\uplus",//"img10.png"
	"\\bigtriangledown",//"img11.png"
	"\\otimes",//"img12.png"
	"\\div",//"img13.png"
	"\\sqcap",//"img14.png"
	"\\triangleleft",//"img15.png"
	"\\oslash",//"img16.png"
	"\\ast",//"img17.png"
	"\\sqcup",//"img18.png"
	"\\triangleright",//"img19.png"
	"\\odot",//"img20.png"
	"\\star",//"img21.png"
	"\\vee",//"img22.png"
	"\\bigcirc",//"img23.png"
	"\\circ",//"img24.png"
	"\\wedge",//"img25.png"
	"\\dagger",//"img26.png"
	"\\bullet",//"img27.png"
	"\\setminus",//"img28.png"
	"\\ddagger",//"img29.png"
	"\\cdot",//"img30.png"
	"\\wr",//"img31.png"
	"\\amalg",//"img32.png"
	"\\leq",//"img33.png"
	"\\geq",//"img34.png"
	"\\equiv",//"img35.png"
	"\\models",//"img36.png"
	"\\prec",//"img37.png"
	"\\succ",//"img38.png"
	"\\sim",//"img39.png"
	"\\perp",//"img40.png"
	"\\preceq",//"img41.png"
	"\\succeq",//"img42.png"
	"\\simeq",//"img43.png"
	"\\mid",//"img44.png"
	"\\ll",//"img45.png"
	"\\gg",//"img46.png"
	"\\asymp",//"img47.png"
	"\\parallel",//"img48.png"
	"\\subset",//"img49.png"
	"\\supset",//"img50.png"
	"\\approx",//"img51.png"
	"\\bowtie",//"img52.png"
	"\\subseteq",//"img53.png"
	"\\supseteq",//"img54.png"
	"\\cong",//"img55.png"
	"\\neq",//"img56.png"
	"\\smile",//"img57.png"
	"\\sqsubseteq",//"img58.png"
	"\\sqsupseteq",//"img59.png"
	"\\doteq",//"img60.png"
	"\\frown",//"img61.png"
	"\\in",//"img62.png"
	"\\ni",//"img63.png"
	"\\propto",//"img64.png"
	"\\vdash",//"img65.png"
	"\\dashv",//"img66.png"
	"\\<",//"img67.png"
	"\\>",//"img68.png"
	"\\sum",//"img69.png"
	"\\bigcap",//"img70.png"
	"\\bigodot",//"img71.png"
	"\\prod",//"img72.png"
	"\\bigcup",//"img73.png"
	"\\bigotimes",//"img74.png"
	"\\coprod",//"img75.png"
	"\\bigsqcup",//"img76.png"
	"\\bigoplus",//"img77.png"
	"\\int",//"img78.png"
	"\\bigvee",//"img79.png"
	"\\biguplus",//"img80.png"
	"\\oint",//"img81.png"
	"\\bigwedge",//"img82.png"
	"\\leqslant",//"img83.png"
	"\\geqslant",//"img84.png"
	"\\lhd",//"img85.png"
	"\\rhd",//"img86.png"
	"\\unlhd",//"img87.png"
	"\\unrhd",//"img88.png"
	"\\Join",//"img89.png"
	"\\sqsubset",//"img90.png"
	"\\sqsupset",//"img91.png"
	"\\dotplus",//"img92.png"
	"\\smallsetminus",//"img93.png"
	"\\Cap",//"img94.png"
	"\\Cup",//"img95.png"
	"\\barwedge",//"img96.png"
	"\\veebar",//"img97.png"
	"\\doublebarwedge",//"img98.png"
	"\\boxminus",//"img99.png"
	"\\boxtimes",//"img100.png"
	"\\boxdot",//"img101.png"
	"\\boxplus",//"img102.png"
	"\\divideontimes",//"img103.png"
	"\\ltimes",//"img104.png"
	"\\rtimes",//"img105.png"
	"\\leftthreetimes",//"img106.png"
	"\\rightthreetimes",//"img107.png"
	"\\curlywedge",//"img108.png"
	"\\curlyvee",//"img109.png"
	"\\circleddash",//"img110.png"
	"\\circledast",//"img111.png"
	"\\circledcirc",//"img112.png"
	"\\centerdot",//"img113.png"
	"\\intercal",//"img114.png"
	"\\leqq",//"img115.png"
	"\\eqslantless",//"img116.png"
	"\\lesssim",//"img117.png"
	"\\lessapprox",//"img118.png"
	"\\approxeq",//"img119.png"
	"\\lessdot",//"img120.png"
	"\\lll",//"img121.png"
	"\\lessgtr",//"img122.png"
	"\\lesseqgtr",//"img123.png"
	"\\lesseqqgtr",//"img124.png"
	"\\doteqdot",//"img125.png"
	"\\risingdotseq",//"img126.png"
	"\\fallingdotseq",//"img127.png"
	"\\backsim",//"img128.png"
	"\\backsimeq",//"img129.png"
	"\\subseteqq",//"img130.png"
	"\\Subset",//"img131.png"
	"\\preccurlyeq",//"img132.png"
	"\\curlyeqprec",//"img133.png"
	"\\precsim",//"img134.png"
	"\\precapprox",//"img135.png"
	"\\vartriangleleft",//"img136.png"
	"\\trianglelefteq",//"img137.png"
	"\\vDash",//"img138.png"
	"\\Vvdash",//"img139.png"
	"\\smallsmile",//"img140.png"
	"\\smallfrown",//"img141.png"
	"\\bumpeq",//"img142.png"
	"\\Bumpeq",//"img143.png"
	"\\geqq",//"img144.png"
	"\\eqslantgtr",//"img145.png"
	"\\gtrsim",//"img146.png"
	"\\gtrapprox",//"img147.png"
	"\\gtrdot",//"img148.png"
	"\\ggg",//"img149.png"
	"\\gtrless",//"img150.png"
	"\\gtreqless",//"img151.png"
	"\\gtreqqless",//"img152.png"
	"\\eqcirc",//"img153.png"
	"\\circeq",//"img154.png"
	"\\triangleq",//"img155.png"
	"\\thicksim",//"img156.png"
	"\\thickapprox",//"img157.png"
	"\\supseteqq",//"img158.png"
	"\\Supset",//"img159.png"
	"\\succcurlyeq",//"img160.png"
	"\\curlyeqsucc",//"img161.png"
	"\\succsim",//"img162.png"
	"\\succapprox",//"img163.png"
	"\\vartriangleright",//"img164.png"
	"\\trianglerighteq",//"img165.png"
	"\\Vdash",//"img166.png"
	"\\shortmid",//"img167.png"
	"\\shortparallel",//"img168.png"
	"\\between",//"img169.png"
	"\\pitchfork",//"img170.png"
	"\\varpropto",//"img171.png"
	"\\blacktriangleleft",//"img172.png"
	"\\therefore",//"img173.png"
	"\\backepsilon",//"img174.png"
	"\\blacktriangleright",//"img175.png"
	"\\because",//"img176.png"
	"\\nless",//"img177.png"
	"\\nleq",//"img178.png"
	"\\nleqslant",//"img179.png"
	"\\nleqq",//"img180.png"
	"\\lneq",//"img181.png"
	"\\lneqq",//"img182.png"
	"\\lvertneqq",//"img183.png"
	"\\lnsim",//"img184.png"
	"\\lnapprox",//"img185.png"
	"\\nprec",//"img186.png"
	"\\npreceq",//"img187.png"
	"\\precnsim",//"img188.png"
	"\\precnapprox",//"img189.png"
	"\\nsim",//"img190.png"
	"\\nshortmid",//"img191.png"
	"\\nmid",//"img192.png"
	"\\nvdash",//"img193.png"
	"\\nvDash",//"img194.png"
	"\\ntriangleleft",//"img195.png"
	"\\ntrianglelefteq",//"img196.png"
	"\\nsubseteq",//"img197.png"
	"\\subsetneq",//"img198.png"
	"\\varsubsetneq",//"img199.png"
	"\\subsetneqq",//"img200.png"
	"\\varsubsetneqq",//"img201.png"
	"\\ngtr",//"img202.png"
	"\\ngeq",//"img203.png"
	"\\ngeqslant",//"img204.png"
	"\\ngeqq",//"img205.png"
	"\\gneq",//"img206.png"
	"\\gneqq",//"img207.png"
	"\\gvertneqq",//"img208.png"
	"\\gnsim",//"img209.png"
	"\\gnapprox",//"img210.png"
	"\\nsucc",//"img211.png"
	"\\nsucceq",//"img212.png"
	"\\succnsim",//"img213.png"
	"\\succnapprox",//"img214.png"
	"\\ncong",//"img215.png"
	"\\nshortparallel",//"img216.png"
	"\\nparallel",//"img217.png"
	"\\nVDash",//"img218.png"
	"\\ntriangleright",//"img219.png"
	"\\ntrianglerighteq",//"img220.png"
	"\\nsupseteq",//"img221.png"
	"\\nsupseteqq",//"img222.png"
	"\\supsetneq",//"img223.png"
	"\\varsupsetneq",//"img224.png"
	"\\supsetneqq",//"img225.png"
	"\\varsupsetneqq",//"img226.png"
	"\\vert",//"img227.png"
	"\\Vert",//"img228.png"
	"\\lbrace",//"img229.png"
	"\\rbrace",//"img230.png"
	"\\lfloor",//"img231.png"
	"\\rfloor",//"img232.png"
	"\\lceil",//"img233.png"
	"\\rceil",//"img234.png"
	"\\langle",//"img235.png"
	"\\rangle",//"img236.png"
	"\\overbrace{}",//"img237.png"
	"\\underbrace{}",//"img238.png"
	"\\overline{}",//"img239.png"
	"\\underline{}",//"img240.png"
	"\\overleftarrow{}",//"img241.png"
	"\\overrightarrow{}",//"img242.png"
	"\\widehat{}",//"img243.png"
	"\\ulcorner",//"img244.png"
	"\\urcorner",//"img245.png"
	"\\llcorner",//"img246.png"
	"\\lrcorner",//"img247.png"
	"\\leftarrow", //"img248.png"
	"\\longleftarrow", //"img249.png"
	"\\uparrow", //"img250.png"
	"\\Leftarrow", //"img251.png"
	"\\Longleftarrow", //"img252.png"
	"\\Uparrow", //"img253.png"
	"\\rightarrow", //"img254.png"
	"\\longrightarrow", //"img255.png"
	"\\downarrow", //"img256.png"
	"\\Rightarrow", //"img257.png"
	"\\Longrightarrow", //"img258.png"
	"\\Downarrow", //"img259.png"
	"\\leftrightarrow", //"img260.png"
	"\\longleftrightarrow", //"img261.png"
	"\\updownarrow", //"img262.png"
	"\\Leftrightarrow", //"img263.png"
	"\\Longleftrightarrow", //"img264.png"
	"\\Updownarrow", //"img265.png"
	"\\mapsto", //"img266.png"
	"\\longmapsto", //"img267.png"
	"\\nearrow", //"img268.png"
	"\\hookleftarrow", //"img269.png"
	"\\hookrightarrow", //"img270.png"
	"\\searrow", //"img271.png"
	"\\leftharpoonup", //"img272.png"
	"\\rightharpoonup", //"img273.png"
	"\\swarrow", //"img274.png"
	"\\leftharpoondown", //"img275.png"
	"\\rightharpoondown", //"img276.png"
	"\\nwarrow", //"img277.png"
	"\\rightleftharpoons", //"img278.png"
	"\\dashrightarrow", //"img279.png"
	"\\dashleftarrow", //"img280.png"
	"\\leftleftarrows", //"img281.png"
	"\\leftrightarrows", //"img282.png"
	"\\Lleftarrow", //"img283.png"
	"\\twoheadleftarrow", //"img284.png"
	"\\leftarrowtail", //"img285.png"
	"\\looparrowleft", //"img286.png"
	"\\leftrightharpoons", //"img287.png"
	"\\curvearrowleft", //"img288.png"
	"\\circlearrowleft", //"img289.png"
	"\\Lsh", //"img290.png"
	"\\upuparrows", //"img291.png"
	"\\upharpoonleft", //"img292.png"
	"\\downharpoonleft", //"img293.png"
	"\\multimap", //"img294.png"
	"\\leftrightsquigarrow", //"img295.png"
	"\\rightrightarrows", //"img296.png"
	"\\rightleftarrows", //"img297.png"
	"\\twoheadrightarrow", //"img298.png"
	"\\rightarrowtail", //"img299.png"
	"\\looparrowright", //"img300.png"
	"\\curvearrowright", //"img301.png"
	"\\circlearrowright", //"img302.png"
	"\\Rsh", //"img303.png"
	"\\downdownarrows", //"img304.png"
	"\\upharpoonright", //"img305.png"
	"\\downharpoonright", //"img306.png"
	"\\rightsquigarrow", //"img307.png"
	"\\nleftarrow", //"img308.png"
	"\\nrightarrow", //"img309.png"
	"\\nLeftarrow", //"img310.png"
	"\\nRightarrow", //"img311.png"
	"\\nleftrightarrow", //"img312.png"
	"\\nLeftrightarrow", //"img313.png"
	"\\leadsto", //"img314.png"
	"\\ldots", //"img315.png"
	"\\cdots", //"img316.png"
	"\\vdots", //"img317.png"
	"\\ddots", //"img318.png"
	"\\aleph", //"img319.png"
	"\\prime", //"img320.png"
	"\\forall", //"img321.png"
	"\\infty", //"img322.png"
	"\\hbar", //"img323.png"
	"\\emptyset", //"img324.png"
	"\\exists", //"img325.png"
	"\\imath", //"img326.png"
	"\\nabla", //"img327.png"
	"\\neg", //"img328.png"
	"\\jmath", //"img329.png"
	"\\surd", //"img330.png"
	"\\flat", //"img331.png"
	"\\triangle", //"img332.png"
	"\\ell", //"img333.png"
	"\\top", //"img334.png"
	"\\natural", //"img335.png"
	"\\clubsuit", //"img336.png"
	"\\wp", //"img337.png"
	"\\bot", //"img338.png"
	"\\sharp", //"img339.png"
	"\\diamondsuit", //"img340.png"
	"\\Re", //"img341.png"
	"\\|", //"img342.png"
	"\\backslash",//"img343.png"
	"\\heartsuit", //"img344.png"
	"\\Im", //"img345.png"
	"\\angle", //"img346.png"
	"\\partial", //"img347.png"
	"\\spadesuit", //"img348.png"
	"\\hslash", //"img349.png"
	"\\vartriangle", //"img350.png"
	"\\triangledown", //"img351.png"
	"\\square", //"img352.png"
	"\\lozenge", //"img353.png"
	"\\circledS", //"img354.png"
	"\\measuredangle", //"img355.png"
	"\\nexists", //"img356.png"
	"\\mho", //"img357.png"
	"\\Finv", //"img358.png"
	"\\Game", //"img359.png"
	"\\Bbbk", //"img360.png"
	"\\backprime", //"img361.png"
	"\\varnothing", //"img362.png"
	"\\blacktriangle", //"img363.png"
	"\\blacktriangledown", //"img364.png"
	"\\blacksquare", //"img365.png"
	"\\blacklozenge", //"img366.png"
	"\\bigstar", //"img367.png"
	"\\sphericalangle", //"img368.png"
	"\\complement", //"img369.png"
	"\\eth", //"img370.png"
	"\\diagup", //"img371.png"
	"\\diagdown", //"img372.png"
	"\\alpha",//"img1greek.png"
	"\\beta",//"img2greek.png"
	"\\gamma",//"img3greek.png"
	"\\delta",//"img4greek.png"
	"\\epsilon",//"img5greek.png"
	"\\varepsilon",//"img6greek.png"
	"\\zeta",//"img7greek.png"
	"\\eta",//"img8greek.png"
	"\\theta",//"img9greek.png"
	"\\vartheta",//"img10greek.png"
	"\\iota",//"img11greek.png"
	"\\kappa",//"img12greek.png"
	"\\lambda",//"img13greek.png"
	"\\mu",//"img14greek.png"
	"\\nu",//"img15greek.png"
	"\\xi",//"img16greek.png"
	"\\pi",//"img17greek.png"
	"\\varpi",//"img18greek.png"
	"\\rho",//"img19greek.png"
	"\\varrho",//"img20greek.png"
	"\\sigma",//"img21greek.png"
	"\\varsigma",//"img22greek.png"
	"\\tau",//"img23greek.png"
	"\\upsilon",//"img24greek.png"
	"\\phi",//"img25greek.png"
	"\\varphi",//"img26greek.png"
	"\\chi",//"img27greek.png"
	"\\psi",//"img28greek.png"
	"\\omega",//"img29greek.png"
	"\\Gamma",//"img30greek.png"
	"\\Delta",//"img31greek.png"
	"\\Theta",//"img32greek.png"
	"\\Lambda",//"img33greek.png"
	"\\Xi",//"img34greek.png"
	"\\Pi",//"img35greek.png"
	"\\Sigma",//"img36greek.png"
	"\\Upsilon",//"img37greek.png"
	"\\Phi",//"img38greek.png"
	"\\Psi",//"img39greek.png"
	"\\Omega"//"img40greek.png"
};

typedef QList<QTableWidgetItem*> usercodelist;

Q_DECLARE_METATYPE(QTableWidgetItem*)

class SymbolListWidget : public QTableWidget {
	Q_OBJECT

public:
	SymbolListWidget(QWidget *parent, int page);
public slots:
	void SetUserPage(usercodelist ulist);

protected:
	void resizeEvent ( QResizeEvent * event );

private:
	QList<QTableWidgetItem*>listOfItems;
};

#endif
