package jhr;

import robocode.AdvancedRobot;
import robocode.ScannedRobotEvent;
import robocode.util.Utils;

public class AlphaBot extends AdvancedRobot {

	public double FORWARD = 1.0;

	@Override
	public void run() {

		// Radar indépendant
		setAdjustRadarForRobotTurn(true);
		setAdjustRadarForGunTurn(true);
		setAdjustGunForRobotTurn(true);

		// Utilisation du "narrow lock" en combat 1-1
		turnRadarRightRadians(Double.POSITIVE_INFINITY);
		do {
			// si rien ne se passe
			turnRadarRight(45);
		} while (true);
	}

	@Override
	public void onScannedRobot(ScannedRobotEvent e) {

		// esquive

		// alignement du radar avec l'adversaire

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
