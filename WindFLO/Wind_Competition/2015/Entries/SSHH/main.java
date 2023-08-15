public class main {
  
  public static void main(String argv[]) {
      try {
        //   WindScenario ws = new WindScenario(argv[0]);
          WindScenario ws = new WindScenario("/Users/lifangsai/Desktop/postgraduation/project/WindFLO/WindFLO/Wind_Competition/2015/Scenarios/2.xml");

          KusiakLayoutEvaluator wfle = new KusiakLayoutEvaluator();
          wfle.initialize(ws);
          SSHH algorithm = new SSHH(wfle);
          algorithm.run(); // optional, name of method 'run' provided on submission
          // algorithm can also just use constructor
      } catch (Exception e) {
          e.printStackTrace();
      }
  }
}
