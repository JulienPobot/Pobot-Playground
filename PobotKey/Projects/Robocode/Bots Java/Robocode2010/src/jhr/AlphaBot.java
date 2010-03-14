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

	// Gestion du sens de d�placement (avancer ou reculer)
	public double FORWARD = 1.0;

	@Override
	public void run() {

		// Le robot, le canon et le radar peuvent tourner ind�pendemment les uns
		// des autres
		setAdjustRadarForRobotTurn(true);
		setAdjustRadarForGunTurn(true);
		setAdjustGunForRobotTurn(true);

		// Utilisation d'un radar de type "narrow lock" en combat 1-1
		// d�crit sur le wiki : http://robowiki.net/wiki/Radar
		turnRadarRightRadians(Double.POSITIVE_INFINITY);
		do {
			// si on a perdu le robot ennemi, on tourne le radar
			// mais �a ne doit pas arriver.
			turnRadarRight(45);
		} while (true);
	}

	@Override
	public void onScannedRobot(ScannedRobotEvent e) {

		// Alignement du radar avec l'adversaire
		// selon le principe "narrow lock"
		// d�crit sur le wiki : http://robowiki.net/wiki/Radar

		double radarTurn =
		// angle entre notre robot (heading) et l'angle duquel l'adversaire est
		// vu (bearing)
		getHeadingRadians() + e.getBearingRadians()
		// moins l'angle d�j� en cours du radar
				- getRadarHeadingRadians();

		// demander � tourner du bon angle
		// en incluant un facteur (1.0, 1.9 ou 2.0)
		// tel que d�crit sur le wiki : http://robowiki.net/wiki/Radar
		setTurnRadarRightRadians(1.9 * Utils.normalRelativeAngle(radarTurn));

		// gestion du canon et du tir
		// selon le mod�le "lin�aire non it�ratif"
		// d�crit sur le wiki : http://robowiki.net/wiki/Linear_Targeting

		// l'angle entre notre robot et l'adversaire
		double absoluteBearing = getHeadingRadians() + e.getBearingRadians();

		// calcul de l'angle de rotation
		double gunTurn = absoluteBearing // enlever l'angle d�j� atteint par le
				// canon
				- getGunHeadingRadians()
				// et prendre en compte la vitesse et la direction de la cible
				// pour tirer vers sa possible prochaine position
				+ (e.getVelocity()
						* Math.sin(e.getHeadingRadians() - absoluteBearing) / 13.0);

		// demander la rotation de l'angle calculer
		setTurnGunRightRadians(Utils.normalRelativeAngle(gunTurn));

		// tirer (au maximum)
		setFire(3.0);

		// g�rer le d�placement du robot
		// en appliquant le principe du mouvement oscillant direct
		// d�crit sur le wiki : http://robowiki.net/wiki/Oscillator_Movement
		// et modifi� pour �tre al�atoire

		// demander le d�placement du robot
		if (getDistanceRemaining() == 0) {
			FORWARD = -FORWARD;
			setAhead(185 * Utils.getRandom().nextInt() * FORWARD);
		}

		// demander la rotation du robot
		setTurnRightRadians(e.getBearingRadians() + Math.PI / 2 - 0.5236
				* FORWARD * (e.getDistance() > 200 ? 1 : -1));

	}
}
