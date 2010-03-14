package jhr;

import robocode.AdvancedRobot;
import robocode.ScannedRobotEvent;
import robocode.util.Utils;

/**
 * 
 * @author Julien pour Pobot
 *
 */
public class AlphaBot extends AdvancedRobot {

	// Gestion du sens de déplacement (avancer ou reculer)
	public double FORWARD = 1.0;

	@Override
	public void run() {

		// Le robot, le canon et le radar peuvent tourner indépendemment les uns des autres
		setAdjustRadarForRobotTurn(true);
		setAdjustRadarForGunTurn(true);
		setAdjustGunForRobotTurn(true);

		// Utilisation d'un radar de type "narrow lock" en combat 1-1
		// décrit sur le wiki : http://robowiki.net/wiki/Radar
		turnRadarRightRadians(Double.POSITIVE_INFINITY);
		do {
			// si on a perdu le robot ennemi, on tourne le radar
			turnRadarRight(45);
		} while (true);
	}

	@Override
	public void onScannedRobot(ScannedRobotEvent e) {

		// Alignement du radar avec l'adversaire
		// selon le principe "narrow lock" 
		// décrit sur le wiki : http://robowiki.net/wiki/Radar

		double radarTurn =
		// Absolute bearing to target
		getHeadingRadians() + e.getBearingRadians()
		// Subtract current radar heading to get turn required
				- getRadarHeadingRadians();

		setTurnRadarRightRadians(1.9 * Utils.normalRelativeAngle(radarTurn));

		// Tourner le gun
			
		double absoluteBearing = getHeadingRadians() + e.getBearingRadians();
		setTurnGunRightRadians(Utils.normalRelativeAngle(absoluteBearing - 
		    getGunHeadingRadians() + (e.getVelocity() * Math.sin(e.getHeadingRadians() - 
		    absoluteBearing) / 13.0)));
		setFire(3.0);
	
		// Se déplacer

		if (getDistanceRemaining() == 0) {
			FORWARD = -FORWARD;
			setAhead(185 * Utils.getRandom().nextInt() * FORWARD);
		}
		setTurnRightRadians(e.getBearingRadians() + Math.PI / 2 - 0.5236
				* FORWARD * (e.getDistance() > 200 ? 1 : -1));


	}

}
